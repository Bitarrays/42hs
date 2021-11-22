#include "lexer.h"

/*static */char **split_in_words(char *input)
{
    // strdup or use input and &input in strtok_r
    char *new = strdup(input);
    char **words = NULL;
    int words_nb = 0;
    char spaces[3] = " \t";
    char *word = NULL;

    while ((word = strtok_r(new, spaces, &new)))
    {
        words = realloc(words, sizeof(char *) * (words_nb + 2));
        words[words_nb] = calloc(strlen(word) + 2, sizeof(char));
        strcpy(words[words_nb], word);
        words_nb++;
    }
    words[words_nb] = NULL;
    free(new);
    return words;
}

struct lexer *lexer_create(char *input)
{
    struct lexer *lexer = calloc(1, sizeof(struct lexer));
    lexer->input = input;
    lexer->tail = NULL;
    lexer->tokens = NULL;
    return lexer;
}

struct lexer_token *lexer_peek(struct lexer *lexer)
{
    return lexer->tokens;
}

struct lexer_token *lexer_pop(struct lexer *lexer)
{
    struct lexer_token *token = lexer->tokens;
    lexer->tokens = lexer->tokens->next;
    return token;
}

void append(struct lexer *lexer, struct lexer_token *token)
{
    token->next = NULL;
    if (lexer->tail)
    {
        lexer->tail->next = token;
        lexer->tail = token;
    }
    else
    {
        lexer->tokens = token;
        lexer->tail = token;
    }
}

void lexer_free(struct lexer *lexer)
{
    struct lexer_token *token = lexer->tokens;
    while (token)
    {
        struct lexer_token *next = token->next;
        free(token->value);
        free(token);
        token = next;
    }
    free(lexer);
}
