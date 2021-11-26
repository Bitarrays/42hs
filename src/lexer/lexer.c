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

static bool is_quote(char c)
{
    return (c == '\'' || c == '\"');
}

static enum token_type get_quote(char c)
{
    if (c == '\'')
        return TOKEN_WORD_SINGLE_QUOTE;
    if (c == '\"')
        return TOKEN_WORD_DOUBLE_QUOTE;
    return TOKEN_ERROR;
}

static void create_token_and_append(char *word, int word_pos, bool *in_cmd,
                                    struct lexer *lexer,
                                    enum token_type *word_type)
{
    if (!word)
        return;
    word[word_pos] = 0;
    struct lexer_token *token = calloc(1, sizeof(struct lexer_token));
    token->type =
        is_keyword(word) && !(*in_cmd) ? get_keyword(word) : *word_type;
    if (token->type >= TOKEN_WORD)
        *in_cmd = true;
    token->value = word;
    word = NULL;
    word_pos = 0;
    lexer_append(lexer, token);
}

static bool is_pipe(char c)
{
    return (c == '|');
}

static void word_lexer(struct lexer *lexer, char *input, bool *in_cmd,
                       enum token_type *word_type)
{
    int j = 0;
    char *word = NULL;
    int word_pos = 0;
    while (input[j])
    {
        if ((*word_type != TOKEN_WORD_SINGLE_QUOTE && is_separator(input[j]))
            || (is_pipe(input[j]) && *word_type == TOKEN_WORD))
        {
            if (word)
            {
                create_token_and_append(word, word_pos, in_cmd, lexer,
                                        word_type);
                word = NULL;
                word_pos = 0;
            }
            struct lexer_token *token = calloc(1, sizeof(struct lexer_token));
            token->type =
                is_separator(input[j]) ? get_separator(input[j]) : TOKEN_PIPE;
            token->value = NULL;
            lexer_append(lexer, token);
            *in_cmd = false;
        }
        else if (is_quote(input[j]))
        {
            if (*word_type == TOKEN_WORD)
                *word_type = get_quote(input[j]);
            else if (get_quote(input[j]) == *word_type)
            {
                create_token_and_append(word, word_pos, in_cmd, lexer,
                                        word_type);
                word = NULL;
                word_pos = 0;
                *word_type = TOKEN_WORD;
            }
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
        create_token_and_append(word, word_pos, in_cmd, lexer, word_type);
        word = NULL;
        word_pos = 0;
    }
    free(input);
}

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
                             "PIPE",
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
        printf("%s ", get_token_string(token->type));
        token = token->next;
    }
    printf("\n");
}

void lexer_build(struct lexer *lexer)
{
    bool in_cmd = false;
    char **words = split_in_words(lexer->input);
    enum token_type word_type = TOKEN_WORD;
    for (int i = 0; words[i]; i++)
    {
        word_lexer(lexer, words[i], &in_cmd, &word_type);
    }
    if (word_type != TOKEN_WORD)
    {
        fprintf(stderr, "Error: quote <%c> is not terminated.\n",
                word_type == TOKEN_WORD_SINGLE_QUOTE ? '\'' : '\"');
    }
    struct lexer_token *token = calloc(1, sizeof(struct lexer_token));
    token->type = TOKEN_EOF;
    lexer_append(lexer, token);
    // lexer_print(lexer);
    free(words);
    lexer->head = lexer->tokens;
}

void lexer_go_back(struct lexer *lexer, struct lexer_token *token)
{
    lexer->head = token;
}
