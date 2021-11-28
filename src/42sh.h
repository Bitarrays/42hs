#ifndef SHELL_H
#define SHELL_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

struct shell
{
    bool pretty_print;
    char *oldpwd;
    char *pwd;
    bool exit;
    char **args;
    int nb_args;
    char *ifs;
    uid_t uid;
    int return_code;
    bool verbose;
    bool interupt;
};

/**
 * @brief Print the shell structure.
 *
 */
void print_shell(void);

/**
 * @brief Free the shell structure.
 *
 */
void free_shell(void);

#endif // !SHELL_H
