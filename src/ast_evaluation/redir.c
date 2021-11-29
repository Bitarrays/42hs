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

static bool has_in_redir(char ***redirs)
{
    int re = 0;
    while (redirs[re])
    {
        int i = 0;
        while (redirs[re][i])
        {
            if (strcmp(redirs[re][i], "<") == 0)
                return true;
            i++;
        }
        return false;
    }
}

int exec_redirections(char **cmd, char ***redirs)
{
    int tmpin = dup(0);
    int tmpout = dup(1);

    // TODO: < redirs
    int fdin;
    if (has_in_redir(redirs))
        return 0;
    else
        fdin = dup(tmpin);

    // pid_t pid = fork();
    // int last_fd = -1;
    // if (pid == 0)
    // {
    //     int redirs_pos = 0;
    //     while (redirs[redirs_pos])
    //     {
    //         char *filename = get_filename_from_redir(redirs[redirs_pos]);
    //         int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    //         int ionumber = get_fd_from_redir(redirs[redirs_pos]);
    //         if (ionumber < 0)
    //         {
    //             close(fd);
    //             redirs_pos++;
    //             continue;
    //         }
    //         dup2(fd, ionumber);
    //         close(fd);
    //         redirs_pos++;
    //     }
    //     if (execvp(cmd[0], cmd))
    //         return (127);
    //     return 0;
    // }
    // else
    // {
    //     int status;
    //     if (waitpid(pid, &status, 0) > 0)
    //     {
    //         if (WIFEXITED(status))
    //         {
    //             if (WEXITSTATUS(status) == 127)
    //             {
    //                 fprintf(stderr, "%s: command not found\n", cmd[0]);
    //                 return 1;
    //             }
    //             printf("%s exited with %d!\n", cmd[0], WEXITSTATUS(status));
    //             return 0;
    //         }
    //         return 1;
    //     }
    //     else
    //         return 1;
    // }
    // return cmd != NULL;
}

int main(void)
{
    char **cmd = calloc(3, sizeof(char *));
    cmd[0] = "ls";
    cmd[1] = "-l";
    char ***redirs = calloc(2, sizeof(char *));
    redirs[0] = calloc(4, sizeof(char *));
    redirs[0][0] = "1";
    redirs[0][1] = ">";
    redirs[0][2] = "err.txt";
    redirs[1] = calloc(4, sizeof(char *));
    redirs[1][0] = "1";
    redirs[1][1] = ">";
    redirs[1][2] = "out.txt";
    exec_redirections(cmd, redirs);
    free(redirs[0]);
    free(redirs);
    free(cmd);
}
