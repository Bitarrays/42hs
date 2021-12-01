#ifndef BUILTINS_H
#define BUILTINS_H

#include <dirent.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "42sh.h"
#include "lexer.h"

extern struct shell *shell;

/**
 * @brief : Check which builtin command is received and execute it.
 *
 * @param toExecute : Builtin command starting from the bultin to the end
 *                      of it's argument.
 * @return int : 0 on success, 1 on failure.
 */
int find_command(char **toExecute);

/**
 * @brief Execute cd command.
 *
 * @param args the list of arguments
 * @return int return code
 */
int cd(char **args);

/**
 * @brief The echo command.
 *
 * @param args the list of arguments
 */
void echo(char **args);

/**
 * @brief  @brief Modify the shell->exit and shell->return_code parameter in the global variable shell
 * 
 * @param args the list of arguments
 * @return int return 0 on success, 1 on failure
 */
int my_exit(char **args);

#endif
