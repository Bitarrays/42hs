#include "42sh.h"

#include <stdio.h>
#include <string.h>

#include "lexer/lexer.h"
#include "shell_input/shell_input.h"

int main(int argc, char **argv)
{
    struct shell *shell = calloc(1, sizeof(struct shell));
    shell->pretty_print = argc > 1 ? !strcmp(argv[1], "--pretty_print") : false;
    int res;
    if (shell->pretty_print)
        res = get_input(argc - 1, argv + 1, shell);
    else
        res = get_input(argc, argv, shell);
    free(shell);
    return res;
    char *input = calloc(49, sizeof(char));
    strcpy(input, "if test then 42sh.c\n then echo '42sh.c exists'\n fi");
    struct lexer *lexer = lexer_create(input);
    lexer_build(lexer);
    lexer_free(lexer);
    return (0);
}
