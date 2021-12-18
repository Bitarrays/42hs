#include <err.h>
#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast_evaluation_tools.h"
#include "ast_evaluation.h"
#include "builtins.h"

int exec_with_fork(struct ast *cmd, int i, int pipe_nb, int ***fds)
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
            int res = evaluate_ast(cmd);
            printf("%d\n", res);
            shell->return_code = res;
            kill(getpid(), SIGKILL);
            return res;
        }
        else if (i == pipe_nb)
        {
            dup2((*fds)[i - 1][0], 0);
            close((*fds)[i - 1][0]);
            close((*fds)[i - 1][1]);
            int res = evaluate_ast(cmd);
            printf("%d\n", res);
            shell->return_code = res;
            kill(getpid(), SIGKILL);
            return res;
        }
        else
        {
            dup2((*fds)[i - 1][0], 0);
            dup2((*fds)[i][1], 1);
            close((*fds)[i - 1][0]);
            close((*fds)[i - 1][1]);
            close((*fds)[i][0]);
            close((*fds)[i][1]);
            int res = evaluate_ast(cmd);
            printf("%d\n", res);
            shell->return_code = res;
            kill(getpid(), SIGKILL);
            return res;
        }
    }
    else
    {
        if (i != 0)
        {
            close((*fds)[i - 1][0]);
            close((*fds)[i - 1][1]);

            int wstatus;
            if (waitpid(pid, &wstatus, 0) == -1)
                shell->return_code = 1;

            if (!WIFEXITED(wstatus))
                shell->return_code = 127;
            else
                shell->return_code = WEXITSTATUS(wstatus);
        }
        return shell->return_code;
    }
    return shell->return_code;
}

int exec_without_fork(char **cmd, int i, int pipe_nb, int ***fds)
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
    return 0;
}

int exec_pipe(struct ast **args, int pipe_nb)
{
    int res = 0;
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
        // if (is_builtin(val[0]))
        //     res = exec_without_fork(val, i, pipe_nb, &fds);
        // else
        exec_with_fork(args[i], i, pipe_nb, &fds);
        res = atoi(find_elt_list(shell, "?"));
    }
    for (int i = 0; i < pipe_nb; i++)
        free(fds[i]);
    free(fds);
    free(args);
    return res;
}
