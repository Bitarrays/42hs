#include "builtins.h"

int find_command(char **args, int fd_write)
{
    if (!strcmp(args[0], "echo"))
    {
        echo(args, fd_write);
        return 0;
    }
    if (!strcmp(args[0], "cd"))
        return cd(args);
    if (!strcmp(args[0], "exit"))
        return my_exit(args);
    else
        return 1;
}
