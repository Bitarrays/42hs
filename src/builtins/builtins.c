#include "builtins.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
        else if (!strcmp(toExecute[start_print], "-ne") && !e_option && !n_option)
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

int find_command(char **toExecute)
{
    if (!strcmp(toExecute[0], "echo"))
    {
        echo(toExecute);
        return 0;
    }
    else
        return 1;
}

/*int main()
{
    char **toExecute = malloc(sizeof(char *) * 10);
    toExecute[0] = "echo";
    toExecute[1] = "-e";
    toExecute[2] = "-n";
    toExecute[3] = "test\n";
    toExecute[4] = NULL;

    find_command(toExecute);

    free(toExecute);

    return 0;
}*/
