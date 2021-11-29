#include "builtins.h"

static void afterBackslash(char *toCheck, int *index)
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
        else if (!strcmp(args[start_print], "-ne") && !e_option && !n_option)
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
