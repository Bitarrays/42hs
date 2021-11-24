#include <stdio.h>

#include "lexer/lexer.h"
#include "shell_input/shell_input.h"

int main(void)
{
    // return get_input(argc, argv);
    char *input = calloc(49, sizeof(char));
    strcpy(input, "if test then 42sh.c\n then echo '42sh.c exists'\n fi");
    struct lexer *lexer = lexer_create(input);
    lexer_build(lexer);
    lexer_free(lexer);
    return (0);
}
