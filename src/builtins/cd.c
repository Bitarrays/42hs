#include "builtins.h"

int cd(char **args)
{
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
