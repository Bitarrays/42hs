#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast_evaluation_tools.h"

/*
 * The main objectif of this source code is to call some programe of the
 * computer using fork and exec
 */

int call_exec(char **cmd)
{
    pid_t cpid = fork();
    if (cpid == -1)
    {
        perror("42sh");
        return 1;
    }
    else if (!cpid)
    {
        return execvp(cmd[0], cmd);
    }

    int cstatus = 0;
    if (waitpid(cpid, &cstatus, 0) == -1)
    {
        perror("42sh");
        return 1;
    }

    if (!WIFEXITED(cstatus))
    {
        perror("42sh");
        return 1;
    }

    printf("%d\n", WEXITSTATUS(cstatus));

    return WEXITSTATUS(cstatus);
}