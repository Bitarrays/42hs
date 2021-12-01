#define _GNU_SOURCE
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

static char *get_filename_from_redir(char **redir)
{
    char *filename = NULL;
    int len = 0;
    while (redir[len])
        len++;
    filename = redir[len - 1];
    return filename;
}

static bool is_int(char *str)
{
    int i = 0;
    while (str[i])
    {
        if (str[i] < '0' || str[i] > '9')
            return false;
        i++;
    }
    return true;
}

static int get_fd_from_redir(char **redir)
{
    if (is_int(redir[0]))
    {
        int res = atoi(redir[0]);
        if (res > 2)
        {
            fprintf(stderr, "42sh: bad file descriptor\n");
            return -1;
        }
        return res;
    }
    return 1;
}

static bool is_in_redir(char **redir)
{
    return (redir[0][0] == '>' || redir[1][0] == '>');
}

static bool is_out_redir(char **redir)
{
    return (redir[0][0] == '<' || redir[1][0] == '<');
}

static int get_open_flags(char **redir)
{
    char *type = redir[0][0] == '>' ? redir[0] : redir[1];
    switch (type[1])
    {
    case 0:
        return O_CREAT | O_WRONLY | O_TRUNC;
    case '>':
        return O_CREAT | O_WRONLY | O_APPEND;
    default:
        return O_CREAT | O_WRONLY | O_TRUNC;
    }
}

int exec_redirections(char **cmd, char ***redirs)
{
    int redirs_pos = 0;
    while (redirs[redirs_pos])
    {
        if (is_in_redir(redirs[redirs_pos]))
        {
            char *filename = get_filename_from_redir(redirs[redirs_pos]);
            int fd = open(filename, get_open_flags(redirs[redirs_pos]), 0644);
            int ionumber = get_fd_from_redir(redirs[redirs_pos]);
            if (ionumber < 0)
            {
                close(fd);
                redirs_pos++;
                continue;
            }
            dup2(fd, ionumber);
            close(fd);
        }
        else if (is_out_redir(redirs[redirs_pos]))
        {
            char *filename = get_filename_from_redir(redirs[redirs_pos]);
            int fd = open(filename, O_RDONLY);
            if (fd == -1)
            {
                fprintf(stderr, "42sh: %s: No such file or directory\n",
                        filename);
                redirs_pos++;
                return -1;
            }
            int ionumber = get_fd_from_redir(redirs[redirs_pos]);
            if (ionumber < 0)
            {
                close(fd);
                redirs_pos++;
                continue;
            }
            dup2(fd, ionumber);
            close(fd);
        }
        redirs_pos++;
    }
    pid_t pid = fork();
    if (pid == 0)
    {
        if (execvp(cmd[0], cmd))
            return (127);
        return 0;
    }
    else
    {
        int status;
        if (waitpid(pid, &status, 0) > 0)
        {
            if (WIFEXITED(status))
            {
                if (WEXITSTATUS(status) == 127)
                {
                    fprintf(stderr, "%s: command not found\n", cmd[0]);
                    return 1;
                }
                printf("%s exited with %d!\n", cmd[0], WEXITSTATUS(status));
                return 0;
            }
            return 1;
        }
        else
            return 1;
    }
    return cmd != NULL;
}

int main(void)
{
    char **cmd = calloc(3, sizeof(char *));
    cmd[0] = "ls";
    cmd[1] = "-l";
    char ***redirs = calloc(2, sizeof(char *));
    redirs[0] = calloc(4, sizeof(char *));
    redirs[0][0] = "1";
    redirs[0][1] = "<";
    redirs[0][2] = "ot.txt";
    exec_redirections(cmd, redirs);
    free(redirs[0]);
    free(redirs);
    free(cmd);
}
