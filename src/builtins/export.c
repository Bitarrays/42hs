#include "../variables/var_list.h"
#include "builtins.h"

int export(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr,
                "42sh: export: expected export <name> or export <name>=<value> "
                "but got export\n");
        return 1;
    }

    for (int i = 0; args[1][i] != '\0'; i++)
    {
        if ((args[1][i] >= 'a' && args[1][i] =< 'z')
            || (args[1][i] >= '0' && args[1][i] =< '9') || args[1][i] == '_')
            continue;

        fprintf(stderr, "42sh: export %s: bad variable name\n", args[1]);
        return 1;
    }

    if (args[2] == NULL)
        setenv(args[1], "", 1);
    else
        setenv(args[1], args[2], 1);

    return 0;
}
