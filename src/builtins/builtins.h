#ifndef BUILTINS_H
#define BUILTINS_H

#include "../lexer/lexer.h"

/**
 * @brief : Check which builtin command is received and execute it.
 * 
 * @param toExecute : Builtin command starting from the bultin to the end
 *                      of it's argument.
 * @return int : 0 on success, 1 on failure.
 */
int find_command(char **toExecute);

#endif
