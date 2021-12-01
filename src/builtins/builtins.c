#include "builtins.h"

int find_command(char **args)
{
    if (!strcmp(args[0], "echo"))
    {
        echo(args);
        return 0;
    }
    if (!strcmp(args[0], "cd"))
        return cd(args);
    if (!strcmp(args[0], "exit"))
        return my_exit(args);
    else
        return 1;
}
