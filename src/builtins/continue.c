#include "builtins.h"

static bool is_int(char *str)
{
    int i = 0;
    while (str[i])
    {
        if (str[i] < '0' || str[i] > '9')
            return false;
        i++;
    }
    return true;
}

int my_continue(char **args)
{
    if (args[1] == NULL)
        return 1;

    if (is_int(args[1]))
        return atoi(args[1]);

    fprintf(stderr, "42sh: continue: Illegal number: %s\n", args[1]);
    return -1;
}
