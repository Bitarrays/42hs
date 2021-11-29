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

void echo(char **args)
{
    bool n_option = false;
    bool e_option = false;
    int start_print = 1;

    for (; args[start_print] != NULL; start_print++)
    {
        if (!strcmp(args[start_print], "-n") && !n_option)
            n_option = true;
        else if (!strcmp(args[start_print], "-e") && !e_option)
            e_option = true;
        else if (!strcmp(args[start_print], "-ne") && !e_option
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
        for (; args[start_print] != NULL; start_print++)
        {
            for (int i = 0; args[start_print][i] != '\0'; i++)
            {
                if (args[start_print][i] == '\\')
                    afterBackslash(args[start_print], &i);
                else
                    putchar(args[start_print][i]);
            }

            if (args[start_print + 1] != NULL)
                putchar(' ');
        }
    }
    else
    {
        for (; args[start_print] != NULL; start_print++)
        {
            for (int i = 0; args[start_print][i] != '\0'; i++)
                putchar(args[start_print][i]);

            if (args[start_print + 1] != NULL)
                putchar(' ');
        }
    }

    if (!n_option)
        printf("\n");
    
    fflush(stdout);
}

int cd(char **args)
{
    if (!args[1] || !strcmp(args[1], "~"))
    {
        shell->oldpwd = strcpy(shell->oldpwd, shell->pwd);
        strcpy(shell->pwd, getenv("HOME"));
        chdir(shell->pwd);
        return 0;
    }
    
    if (!strcmp(args[1], "-"))
    {
        chdir(shell->oldpwd);
        char *swap = shell->oldpwd;
        shell->oldpwd = shell->pwd;
        shell->pwd = swap;
        printf("%s\n", shell->pwd);
        fflush(stdout);
        return 0;
    }

    int error_chdir = chdir(args[1]);

    if (error_chdir == -1)
    {
        fprintf(stderr, "42sh: cd: can't cd to %s\n", args[1]);
        return 1;
    }
    
    shell->oldpwd = strcpy(shell->oldpwd, shell->pwd);

    getcwd(shell->pwd, 2048);

    return 0;
}

int find_command(char **args)
{
    if (!strcmp(args[0], "echo"))
    {
        echo(args);
        return 0;
    }
    if (!strcmp(args[0], "cd"))
        return cd(args);
    else
        return 1;
}
