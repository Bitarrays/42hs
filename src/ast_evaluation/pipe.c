#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ast_evaluation_tools.h"

/*int process_cmd(char **cmd, enum quotes *enclosure)
{
    char **val = expand(ast->value, ast->enclosure);
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

void exec_pipe(char ***args, int pipe_nb)
{
    int **fds = calloc(pipe_nb, sizeof(int *));
    int pid;

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

        if ((pid = fork()) == -1)
        {
            fprintf(stderr, "42sh: fork error\n");
        }
        else if (pid == 0)
        {
            if (i == 0)
            {
                dup2(fds[i][1], 1);
                close(fds[i][0]);
                close(fds[i][1]);
                execvp(args[i][0], args[i]);
            }
            else if (i == pipe_nb)
            {
                dup2(fds[i - 1][0], 0);
                close(fds[i - 1][0]);
                close(fds[i - 1][1]);
                execvp(args[i][0], args[i]);
            }
            else
            {
                dup2(fds[i - 1][0], 0);
                dup2(fds[i][1], 1);
                close(fds[i - 1][0]);
                close(fds[i - 1][1]);
                close(fds[i][0]);
                close(fds[i][1]);
                execvp(args[i][0], args[i]);
            }
        }
        else if (i != 0)
        {
            printf("%d\n", i);
            close(fds[i - 1][0]);
            close(fds[i - 1][1]);
            if (i == pipe_nb)
            {
                fprintf(stderr,
                        "pipe.c: TODO: get return code of the last pipe using "
                        "WAITPID\n");
            }
        }
    }
    for (int i = 0; i < pipe_nb; i++)
        free(fds[i]);
    free(fds);
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