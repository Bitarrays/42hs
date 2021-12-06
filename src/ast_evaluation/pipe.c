#include <err.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast_evaluation_tools.h"
#include "builtins.h"

/*
int process_cmd(char **cmd, enum quotes *enclosure)
{
    char **val = expand(cmd, enclosure);
    // val = split_arg(val, ast->enclosure);
    if (!val)
        return 1;
    int res;
    if (is_builtin(*(val)))
        res = find_command(val);
    else
        res = call_exec(val);
    char *tmp = val[0];
    int pos = 0;
    while (tmp)
    {
        free(tmp);
        tmp = val[++pos];
    }
    free(val);
    return res;
}*/

void exec_with_fork(char **cmd, int i, int pipe_nb, int ***fds)
{
    int pid = fork();
    if (pid == -1)
    {
        fprintf(stderr, "42sh: fork error\n");
    }
    else if (pid == 0)
    {
        if (i == 0)
        {
            dup2((*fds)[i][1], 1);
            close((*fds)[i][0]);
            close((*fds)[i][1]);
            execvp(cmd[0], cmd);
        }
        else if (i == pipe_nb)
        {
            dup2((*fds)[i - 1][0], 0);
            close((*fds)[i - 1][0]);
            close((*fds)[i - 1][1]);
            execvp(cmd[0], cmd);
        }
        else
        {
            dup2((*fds)[i - 1][0], 0);
            dup2((*fds)[i][1], 1);
            close((*fds)[i - 1][0]);
            close((*fds)[i - 1][1]);
            close((*fds)[i][0]);
            close((*fds)[i][1]);
            execvp(cmd[0], cmd);
        }
    }
    if (i != 0)
    {
        close((*fds)[i - 1][0]);
        close((*fds)[i - 1][1]);
        if (i == pipe_nb)
        {
            int wstatus;
            if (waitpid(pid, &wstatus, 0) == -1)
                fprintf(stderr, "42sh: waitpid error\n");
            shell->return_code = WEXITSTATUS(wstatus);
        }
    }
}

void exec_without_fork(char **cmd, int i, int pipe_nb, int ***fds)
{
    if (i == 0)
    {
        dup2((*fds)[i][1], 1);
        find_command(cmd, (*fds)[i][1]);
        close((*fds)[i][0]);
        close((*fds)[i][1]);
    }
    else if (i == pipe_nb)
    {
        dup2((*fds)[i - 1][0], 0);
        find_command(cmd, 1);
        close((*fds)[i - 1][0]);
        close((*fds)[i - 1][1]);
    }
    else
    {
        dup2((*fds)[i - 1][0], 0);
        dup2((*fds)[i][1], 1);
        find_command(cmd, 1);
        close((*fds)[i - 1][0]);
        close((*fds)[i - 1][1]);
        close((*fds)[i][0]);
        close((*fds)[i][1]);
    }
    if (i != 0)
    {
        if (i == pipe_nb)
            shell->return_code = find_command(cmd, 1);
        close((*fds)[i - 1][0]);
        close((*fds)[i - 1][1]);
    }
}

void exec_pipe(char ***args, int pipe_nb)
{
    int **fds = calloc(pipe_nb, sizeof(int *));

    for (int i = 0; i < pipe_nb; i++)
        fds[i] = calloc(2, sizeof(int));

    for (int i = 0; i <= pipe_nb; i++)
    {
        if (i != pipe_nb)
        {
            if (pipe(fds[i]) == -1)
            {
                fprintf(stderr, "42sh: bad pipe\n");
            }
        }
        if (is_builtin(args[i][0]))
            exec_without_fork(args[i], i, pipe_nb, &fds);
        else
            exec_with_fork(args[i], i, pipe_nb, &fds);
    }
    for (int i = 0; i < pipe_nb; i++)
        free(fds[i]);
    free(fds);
    free(args);
}

/*int main(void)
{
    char ***args = calloc(4, sizeof(char **));
    args[0] = calloc(3, sizeof(char *));
    args[0][0] = "ls";
    args[0][1] = "-l";
    args[0][2] = NULL;
    args[2] = calloc(4, sizeof(char *));
    args[2][0] = "tail";
    args[2][1] = "-n";
    args[2][2] = "3";
    args[2][3] = NULL;
    args[1] = calloc(2, sizeof(char *));
    args[1][0] = "uniq";
    args[1][1] = NULL;
    args[3] = calloc(4, sizeof(char *));
    args[3][0] = "tail";
    args[3][1] = "-n";
    args[3][2] = "4";
    args[3][3] = NULL;
    exec_pipe(args, 3);
    free(args[0]);
    free(args[1]);
    free(args[2]);
    free(args[3]);
    free(args);
}
*/