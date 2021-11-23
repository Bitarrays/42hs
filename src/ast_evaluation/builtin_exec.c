#include "ast_evaluation_tools.h"
#include "../builtins/builtins.h"

/*
 * The main objectif of this source code is to call the builtin implemented inside of the 42sh project
 */

int call_echo(char *cmd)
{
    int i = 0;
    int flag = 0;
    while (cmd[i] != '\0')
    {
        while (cmd[i] > 31)
            i++;
        if (!strncmp(cmd, "-e", i))
            if (i != 1 && i != 3)
                flag++;
        if (!strncmp(cmd, "-n", i))
            if (i < 2)
                flag += 2;
        if (!strncmp(cmd, "-ne", i))
            flag = 3;
        i++;
    }
    echo(cmd + i, flag);
    return 0;
}

int call_builtin(char *cmd)
{
    int i = 0;
    while (s[i] > 31)
        i++;
    if (!strncmp(cmd, "echo", i))
    {
        return call_echo(cmd + i);
    }
    return 0;
}