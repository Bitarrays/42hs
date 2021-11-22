#include "lexer.h"

struct lexer_token *lexer_token_free(struct lexer_token *token)
{
    free(token->value);
    free(token);
    return NULL;
}

static char **split_in_words(char *input)
{
    // strdup or use input and &input in strtok_r
    char *new = strdup(input);
    char *save = new;
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
    free(save);
    return words;
}

struct lexer *lexer_create(char *input)
{
    struct lexer *lexer = calloc(1, sizeof(struct lexer));
    lexer->input = input;
    lexer->tail = NULL;
    lexer->head = NULL;
    lexer->tokens = NULL;
    return lexer;
}

struct lexer_token *lexer_peek(struct lexer *lexer)
{
    return lexer->head;
}

struct lexer_token *lexer_pop(struct lexer *lexer)
{
    struct lexer_token *token = lexer->head;
    lexer->head = lexer->head->next;
    return token;
}

void lexer_append(struct lexer *lexer, struct lexer_token *token)
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
        lexer_token_free(token);
        token = next;
    }
    lexer->head = NULL;
    lexer->tail = NULL;
    free(lexer->input);
    free(lexer);
}

static bool is_keyword(char *word)
{
    return (!strcmp(word, "if") || !strcmp(word, "else")
            || !strcmp(word, "elif") || !strcmp(word, "fi")
            || !strcmp(word, "then"));
}

static enum token_type get_keyword(char *word)
{
    if (!strcmp(word, "if"))
        return TOKEN_IF;
    if (!strcmp(word, "else"))
        return TOKEN_ELSE;
    if (!strcmp(word, "elif"))
        return TOKEN_ELIF;
    if (!strcmp(word, "fi"))
        return TOKEN_FI;
    if (!strcmp(word, "then"))
        return TOKEN_THEN;
    return TOKEN_ERROR;
}

static bool is_separator(char c)
{
    return (c == ';' || c == '\n');
}

static enum token_type get_separator(char c)
{
    if (c == ';')
        return TOKEN_SEMICOLON;
    if (c == '\n')
        return TOKEN_NEWLINE;
    return TOKEN_ERROR;
}

static void word_lexer(struct lexer *lexer, char *input)
{
    int j = 0;
    char *word = NULL;
    int word_pos = 0;
    while (input[j])
    {
        if (is_separator(input[j]))
        {
            if (word)
            {
                word[word_pos] = 0;
                struct lexer_token *token =
                    calloc(1, sizeof(struct lexer_token));
                token->type = is_keyword(word) ? get_keyword(word) : TOKEN_WORD;
                token->value = word;
                word = NULL;
                word_pos = 0;
                lexer_append(lexer, token);
            }
            struct lexer_token *token = calloc(1, sizeof(struct lexer_token));
            token->type = get_separator(input[j]);
            token->value = NULL;
            lexer_append(lexer, token);
        }
        else
        {
            word = realloc(word, (word_pos + 2) * sizeof(char));
            word[word_pos++] = input[j];
        }
        j++;
    }
    if (word)
    {
        word[word_pos] = 0;
        struct lexer_token *token = calloc(1, sizeof(struct lexer_token));
        token->type = is_keyword(word) ? get_keyword(word) : TOKEN_WORD;
        token->value = word;
        word = NULL;
        word_pos = 0;
        lexer_append(lexer, token);
    }
    free(input);
}

void lexer_print(struct lexer *lexer)
{
    struct lexer_token *token = lexer->tokens;
    while (token)
    {
        printf("%d\n", token->type);
        token = token->next;
    }
}

void lexer_build(struct lexer *lexer)
{
    char **words = split_in_words(lexer->input);
    for (int i = 0; words[i]; i++)
    {
        word_lexer(lexer, words[i]);
    }
    struct lexer_token *token = calloc(1, sizeof(struct lexer_token));
    token->type = TOKEN_EOF;
    lexer_append(lexer, token);
    lexer_print(lexer);
    free(words);
}

void lexer_go_back(struct lexer *lexer, struct lexer_token *token)
{
    lexer->head = token;
}
