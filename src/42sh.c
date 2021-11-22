#include <stdio.h>

#include "lexer/lexer.h"

int main(void)
{
    char *input = calloc(49, sizeof(char));
    strcpy(input, "if test -f 42sh.c; then echo '42sh.c exists'; fi");
    struct lexer *lexer = lexer_create(input);
    lexer_build(lexer);
    lexer_free(lexer);
    // printf("Done.\n");
    return (0);
}
