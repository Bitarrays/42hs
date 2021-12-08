#ifndef SHELL_H
#define SHELL_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

struct var
{
    char *name;
    char *value;
    struct var *next;
};

struct var_stack
{
    struct var *var_list;
    struct var_stack *next;
};

struct loop_stack
{
    struct ast *loop;
    char **var;
    int index;
    struct loop_stack *next;
};

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
    struct var_stack *var_stack;
    struct var *var_list;
    struct loop_stack *loop_stack;
    pid_t pid;
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
