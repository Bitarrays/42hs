#ifndef SHELL_INPUT_H
#define SHELL_INPUT_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "../42sh.h"

/**
 * @brief Get the input for the shell and send it to the lexer
 *
 * @param argc number of arguments
 * @param argv list of arguments
 * @return int
 */
int get_input(int argc, char **argv, struct shell *shell);

#endif // !SHELL_INPUT_H
