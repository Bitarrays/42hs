#include "lexer.h"

static char *get_token_string(enum token_type type)
{
    char *token_string[] = { "ERROR",
                             "IF",
                             "ELSE",
                             "ELIF",
                             "FI",
                             "THEN",
                             "SEMICOLON",
                             "NEWLINE",
                             "REDIR",
                             "IONUMBER",
                             "PIPE",
                             "NOT",
                             "WORD",
                             "WORD_SINGLE_QUOTE",
                             "WORD_DOUBLE_QUOTE",
                             "EOF" };
    return token_string[type];
}

void lexer_print(struct lexer *lexer)
{
    printf("lexer output: ");
    struct lexer_token *token = lexer->tokens;
    while (token)
    {
        if (token->type != TOKEN_REDIR)
            printf("%s ", get_token_string(token->type));
        else
            printf("%s ", token->value);
        token = token->next;
    }
    printf("\n");
}