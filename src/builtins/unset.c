#include "builtins.h"

int unset(char **args)
{
    bool v_option = false;
    bool f_option = false;
    int start_print = 1;

    for (; args[start_print] != NULL; start_print++)
    {
        if (!strcmp(args[start_print], "-v"))
            v_option = true;
        else if (!strcmp(args[start_print], "-f"))
            f_option = true;
        else if (!strcmp(args[start_print], "-fv") || !strcmp(args[start_print], "-vf"))
        {
            v_option = true;
            f_option = true;
        }
        else
        {
            if (args[start_print][0] == '-')
            {
                fprintf(stderr, "42sh: unset: Illegal option %s\n", args[start_print]);
                return 1;
            }

            break;
        }
    }

    if (!v_option && !f_option)
        v_option = true;

    if (v_option)
        //TODO: Delete the named variable in args[start_print] if it exists. No possible error.

    if (f_option)
        //TODO: Delete the named function in args[start_print] if it exists. No possible error.
    
    return 0;
}
