#include "lexer.h"

#include "../42sh.h"
#include "lexer_tools.h"

extern struct shell *shell;

struct lexer_token *lexer_token_free(struct lexer_token *token)
{
    free(token->value);
    free(token);
    return NULL;
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

static void create_word_and_append(char *word, int word_pos, bool *in_cmd,
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

static bool is_pipe(char c, char next)
{
    return (c == '|' && next != '|');
}

static bool is_redir(char c1)
{
    return (c1 == '<' || c1 == '>');
}

static char *get_redir(char c1, char c2)
{
    char *res = calloc(3, sizeof(char));
    if (c1 == '<')
    {
        res[0] = '<';
        if (c2 == '&' || c2 == '>')
            res[1] = c2;
    }
    if (c1 == '>')
    {
        res[0] = '>';
        if (c2 == '&' || c2 == '>' || c2 == '|')
            res[1] = c2;
    }
    return res;
}

static void word_lexer(struct lexer *lexer, char *input, bool *in_cmd,
                       enum token_type *word_type)
{
    int j = 0;
    char *word = NULL;
    int word_pos = 0;
    if (*word_type == TOKEN_WORD && !strcmp(input, "in") && !lexer->in_for)
    {
        create_and_append_token(lexer, TOKEN_IN, NULL);
        lexer->in_for = true;
        free(input);
        return;
    }
    while (input[j])
    {
        if (input[j] == '\\')
        {
            word = realloc(word, (word_pos + 3) * sizeof(char));
            word[word_pos++] = input[j++];
            if (input[j] == 0)
                break;
            word[word_pos++] = input[j++];
            if (input[j] == 0)
                break;
        }
        if ((*word_type != TOKEN_WORD_SINGLE_QUOTE && is_separator(input[j]))
            || (is_pipe(input[j], input[j + 1]) && *word_type == TOKEN_WORD))
        {
            if (word)
            {
                create_word_and_append(word, word_pos, in_cmd, lexer,
                                       word_type);
                word = NULL;
                word_pos = 0;
            }
            create_and_append_token(
                lexer,
                is_separator(input[j]) ? get_separator(input[j]) : TOKEN_PIPE,
                NULL);
            if (is_separator(input[j]))
                lexer->in_for = true;
            *in_cmd = false;
        }
        else if (*word_type == TOKEN_WORD
                 && ((input[j] == '&' && input[j + 1] == '&')
                     || (input[j] == '|' && input[j + 1] == '|')))
        {
            if (word)
            {
                create_word_and_append(word, word_pos, in_cmd, lexer,
                                       word_type);
                word = NULL;
                word_pos = 0;
            }
            create_and_append_token(
                lexer, input[j] == '&' ? TOKEN_AND : TOKEN_OR, NULL);
            j++;
        }
        else if (*word_type == TOKEN_WORD && is_redir(input[j]))
        {
            if (word)
            {
                word[word_pos] = 0;
                if (is_int(word))
                {
                    create_and_append_token(lexer, TOKEN_IONUMBER, word);
                }
                else
                    create_word_and_append(word, word_pos, in_cmd, lexer,
                                           word_type);
                word = NULL;
                word_pos = 0;
            }
            create_and_append_token(lexer, TOKEN_REDIR,
                                    get_redir(input[j], input[j + 1]));
            if (input[j + 1] != 0)
                j++;
        }
        else if (*word_type == TOKEN_WORD && input[j] == '='
                 && (!lexer->tail
                     || lexer->tail->type != TOKEN_ASSIGNMENT_WORD))
        {
            if (word)
            {
                create_word_and_append(word, word_pos, in_cmd, lexer,
                                       word_type);
                word = NULL;
                word_pos = 0;
                lexer->tail->type = TOKEN_ASSIGNMENT_WORD;
            }
        }
        else if (is_quote(input[j]))
        {
            // enum token_type before = *word_type;
            if (*word_type == TOKEN_WORD)
                *word_type = get_quote(input[j]);
            else if (get_quote(input[j]) == *word_type)
            {
                create_word_and_append(word, word_pos, in_cmd, lexer,
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
        create_word_and_append(word, word_pos, in_cmd, lexer, word_type);
        word = NULL;
        word_pos = 0;
    }
    free(input);
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
        shell->exit = true;
    }
    create_and_append_token(lexer, TOKEN_EOF, NULL);
    if (shell->verbose)
        lexer_print(lexer);
    free(words);
    lexer->head = lexer->tokens;
}

void lexer_go_back(struct lexer *lexer, struct lexer_token *token)
{
    lexer->head = token;
}
