#include <stdio.h>

#include "builtins.h"

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

        *index += 1;
    }
    else
        putchar('\\');
}


//Pour l'instant je ne retourne pas d'erreur donc je passe echo en void
void echo(char *toPrint, enum flags_echo flag)
{
    if (flag == E_OPTION)
    {
        for (int i = 0; toPrint[i] != '\0'; i++)
        {
            if (toPrint[i] == '\\')
                afterBackslash(toPrint, &i);
            else
                putchar(toPrint[i]);
        }
    }
    else
    {
        for (int i = 0; toPrint[i] != '\0'; i++)
            putchar(toPrint[i]);
    }

    if (flag != N_OPTION && flag != N_E_OPTIONS)
        printf("\n");
}

int main(int argc, char **argv)
{
    enum flags_echo flags = E_OPTION;
    echo(argv[1], flags);
}
