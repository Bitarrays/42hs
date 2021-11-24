#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "ast_evaluation_tools.h"

/*
 * The main objectif of this source code is to call some programe of the
 * computer using fork and exec
 */

int call_exec(char *name)
{
    pid_t cpid = fork();
    if (cpid == -1)
    {
        perror("42sh");
        return 1;
    }
    else if (!cpid)
    {
        return execvp(name, NULL);
    }

    int cstatus = 0;
    if (waitpid(cpid, &cstatus, 0) == -1)
    {
        perror("42sh");
        return 1;
    }

    if (!(WIFEEXITED(cstatus) && WEXITSTATUS(cstatus) == 0))
    {
        perror("42sh");
        return 1;
    }

    return 0;
}