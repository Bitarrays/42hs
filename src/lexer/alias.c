#include "42sh.h"
#include "lexer_tools.h"

extern struct shell *shell;

struct lexer_alias *get_alias(char *name)
{
    struct lexer_alias *head = shell->alias_list;
    while (head)
    {
        if (head->name && !strcmp(head->name, name))
            return head;
        head = head->next;
    }
    return NULL;
}

static char *get_token_string(enum token_type type)
{
    char *token_string[] = { "",
                             "if",
                             "else",
                             "elif",
                             "fi",
                             "then",
                             "do",
                             "done",
                             "while",
                             "until",
                             "for",
                             "in",
                             "&&",
                             "||",
                             ";",
                             "\n",
                             "REDIR",
                             "IONUMBER",
                             "|",
                             "!",
                             "ASSIGNMENT_WORD",
                             "(",
                             "$(",
                             ")",
                             "{",
                             "}",
                             "$",
                             "case",
                             "esac",
                             "WORD",
                             "WORD_DOUBLE_QUOTE",
                             "WORD_SINGLE_QUOTE",
                             "EOF",
                             " " };
    return strdup(token_string[type]);
}

static char *get_alias_value(struct lexer_token *head)
{
    char *res = NULL;
    while (head)
    {
        size_t len = res ? strlen(res) : 0;
        char *to_append =
            head->value ? head->value : get_token_string(head->type);
        res = realloc(res, sizeof(char) * (len + strlen(to_append) + 1));
        res[len] = '\0';
        strcat(res, to_append);
        if (!head->value)
            free(to_append);
        head = head->next;
    }
    return res;
}

void lexer_append_alias(struct lexer *lexer, struct lexer_alias *alias)
{
    struct lexer_token *token = alias->value;
    while (token)
    {
        struct lexer_token *new = calloc(1, sizeof(struct lexer_token));
        new->type = token->type;
        if (token->value)
            new->value = strdup(token->value);
        else
            new->value = NULL;
        lexer_append(lexer, new);
        token = token->next;
    }
}

struct lexer_token *copy_lexer_alias(struct lexer_token *head)
{
    struct lexer_token *new_head = calloc(1, sizeof(struct lexer_token));
    new_head->type = head->type;
    if (head->value)
        new_head->value = strdup(head->value);
    head = head->next;
    struct lexer_token *current_tail = head;
    while (head)
    {
        struct lexer_token *new_token = calloc(1, sizeof(struct lexer_token));
        new_token->type = head->type;
        if (head->value)
            new_token->value = strdup(head->value);
        current_tail->next = new_token;
        current_tail = new_token;
        head = head->next;
    }
    return new_head;
}

static void process_single_alias(struct lexer *lexer, char *name,
                                 struct lexer_token *value)
{
    if (value)
    {
        struct lexer_alias *alias = calloc(1, sizeof(struct lexer_alias));
        alias->name = strdup(name);
        alias->value = value;
        alias->next = lexer->alias_list;
        lexer->alias_list = alias;
        shell->return_code = 0;
    }
    else
    {
        struct lexer_alias *alias = get_alias(name);
        if (!alias)
        {
            shell->return_code = 1;
            fprintf(stderr, "42sh: alias: %s: not found\n", name);
        }
        else
        {
            char *a_value = get_alias_value(alias->value);
            printf("%s='%s'\n", alias->name, a_value);
            free(a_value);
            shell->return_code = 0;
        }
    }
}

void process_alias(struct lexer_token *prev, struct lexer_token *head,
                   struct lexer *lexer)
{
    if (!head
        || (head->type != TOKEN_ALIAS && head->type != TOKEN_SPACE
            && head->type != TOKEN_ASSIGNMENT_WORD && head->type != TOKEN_WORD
            && head->type != TOKEN_WORD_SINGLE_QUOTE
            && head->type != TOKEN_WORD_DOUBLE_QUOTE))
    {
        if (head && (head->type == TOKEN_SEMICOLON || head->type == TOKEN_NEWLINE))
        {
            if (prev)
                prev->next = head->next;
            else
                lexer->tokens = head->next;
            lexer_token_free(head);
        }
        return;
    }
    struct lexer_token *name = head;
    if (head->type == TOKEN_ALIAS)
    {
        name = head->next;
        lexer_token_free(head);
    }
    while (name->type == TOKEN_SPACE)
    {
        struct lexer_token *next = name->next;
        lexer_token_free(name);
        name = next;
    }
    head = name;
    struct lexer_token *value = NULL;
    struct lexer_token *end = head->next;
    if (name->type == TOKEN_ASSIGNMENT_WORD)
    {
        value = head->next;
        while (value->type == TOKEN_SPACE)
        {
            struct lexer_token *next = value->next;
            lexer_token_free(value);
            value = next;
        }
        end = value->next;
        struct lexer_token *previous = value;
        while (end->type != TOKEN_SPACE && end->type != TOKEN_NEWLINE
               && end->type != TOKEN_EOF && end->type != TOKEN_SEMICOLON)
        {
            previous = end;
            end = end->next;
        }
        previous->next = NULL;
        if (prev)
            prev->next = end;
        else
            lexer->tokens = end;
    }
    else
    {
        if (prev)
            prev->next = end;
        else
            lexer->tokens = end;
    }
    process_single_alias(lexer, name->value, value);
    lexer_token_free(name);
    if (end->type == TOKEN_SEMICOLON)
        end->type = TOKEN_SPACE;
    process_alias(end, end ? end->next : NULL, lexer);
}
