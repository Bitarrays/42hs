#include "builtins.h"

#include <dirent.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "../42sh.h"
#define BUFSIZE 400
extern struct shell *shell;

void afterBackslash(char *toCheck, int *index)
{
    *index += 1;

    if (toCheck[*index] != '\0')
    {
        switch (toCheck[*index])
        {
        case '\\':
            putchar('\\');
            break;
        case 'n':
            putchar('\n');
            break;
        case 't':
            putchar('\t');
            break;
        default:
            putchar('\\');
            putchar(toCheck[*index]);
            break;
        }
    }
    else
        putchar('\\');
}

void echo(char **toExecute)
{
    bool n_option = false;
    bool e_option = false;
    int start_print = 1;

    for (; toExecute[start_print] != NULL; start_print++)
    {
        if (!strcmp(toExecute[start_print], "-n") && !n_option)
            n_option = true;
        else if (!strcmp(toExecute[start_print], "-e") && !e_option)
            e_option = true;
        else if (!strcmp(toExecute[start_print], "-ne") && !e_option
                 && !n_option)
        {
            e_option = true;
            n_option = true;
            break;
        }
        else
            break;
    }

    if (e_option)
    {
        for (; toExecute[start_print] != NULL; start_print++)
        {
            for (int i = 0; toExecute[start_print][i] != '\0'; i++)
            {
                if (toExecute[start_print][i] == '\\')
                    afterBackslash(toExecute[start_print], &i);
                else
                    putchar(toExecute[start_print][i]);
            }

            if (toExecute[start_print + 1] != NULL)
                putchar(' ');
        }
    }
    else
    {
        for (; toExecute[start_print] != NULL; start_print++)
        {
            for (int i = 0; toExecute[start_print][i] != '\0'; i++)
                putchar(toExecute[start_print][i]);

            if (toExecute[start_print + 1] != NULL)
                putchar(' ');
        }
    }

    if (!n_option)
        printf("\n");
}

int cd(char **toExecute)
{
    if (toExecute[1] == NULL)
        return 0;
    
    if (!strcmp(toExecute[1], "-"))
    {
        chdir(shell->oldpwd);
        char *swap = shell->oldpwd;
        shell->oldpwd = shell->pwd;
        shell->pwd = swap;
        return 0;
    }

    int error_chdir = chdir(toExecute[1]);

    if (error_chdir == -1)
    {
        fprintf(stderr, "42sh: cd: can't cd to %s\n", toExecute[1]);
        return 1;
    }
    
    shell->oldpwd = strcpy(shell->oldpwd, shell->pwd);

    if (shell->pwd[strlen(shell->pwd) - 1] != '/')
        shell->pwd = strcat(shell->pwd, "/");

    shell->pwd = strcat(shell->pwd, toExecute[1]);

    return 0;
}

int find_command(char **toExecute)
{
    if (!strcmp(toExecute[0], "echo"))
    {
        echo(toExecute);
        return 0;
    }
    if (!strcmp(toExecute[0], "cd"))
        return cd(toExecute);
    else
        return 1;
}
