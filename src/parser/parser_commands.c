#include "parser.h"

enum parser_status parse_redirection(struct ast **ast,
                                            struct lexer *lexer)
{
    struct lexer_token *tok = lexer_peek(lexer);
    char *type = NULL;
    size_t len = 0;

    // Try [IONUMBER]
    if (tok->type == TOKEN_IONUMBER)
    {
        len = strlen(tok->value);
        type = realloc(type, len + 1);
        type = strcpy(type, tok->value);
        type[len] = '\0';
        lexer_pop(lexer);
    }

    tok = lexer_peek(lexer);
    if (tok->type != TOKEN_REDIR)
    {
        free(type);
        return handle_parser_error(PARSER_ERROR, ast);
    }

    len += strlen(tok->value);
    if (!type)
        type = calloc(len + 1, sizeof(char));
    else
        type = realloc(type, len + 1);
    type[len] = '\0';
    type = strcat(type, tok->value);
    *ast = ast_new(AST_REDIR);
    char **redir_value = calloc(2, sizeof(char *));
    redir_value[0] = type;
    (*ast)->value = redir_value;

    lexer_pop(lexer);
    tok = lexer_peek(lexer);
    if (tok->type != TOKEN_WORD && tok->type != TOKEN_WORD_SINGLE_QUOTE && tok->type != TOKEN_WORD_DOUBLE_QUOTE)
        return handle_parser_error(PARSER_ERROR, ast);

    struct ast *ast_word = ast_new(AST_COMMAND);
    char **word_value = calloc(2, sizeof(char *));
    word_value[0] = tok->value;
    enum quotes *word_q = calloc(1, sizeof(enum quotes));
    if (tok->type == TOKEN_WORD)
        word_q[0] = Q_NONE;
    else if (tok->type == TOKEN_WORD_SINGLE_QUOTE)
        word_q[0] = Q_SINGLE;
    else if (tok->type == TOKEN_WORD_DOUBLE_QUOTE)
        word_q[0] = Q_DOUBLE;

    ast_word->value = word_value;
    ast_word->enclosure = word_q;
    (*ast)->right_child = ast_word;

    lexer_pop(lexer);

    return PARSER_OK;
}

static enum parser_status parse_prefix(struct ast **ast, struct lexer *lexer)
{
    // Try redirection
    enum parser_status status_redir = parse_redirection(ast, lexer);
    if (status_redir == PARSER_ERROR)
        return handle_parser_error(status_redir, ast);

    return PARSER_OK;
}

static enum parser_status parse_element(struct ast **ast, struct lexer *lexer)
{
    // Try WORD
    struct lexer_token *tok = lexer_peek(lexer);
    if (tok->type == TOKEN_WORD || tok->type == TOKEN_WORD_DOUBLE_QUOTE
        || tok->type == TOKEN_WORD_SINGLE_QUOTE)
    {
        *ast = ast_new(AST_COMMAND);
        char **value = calloc(2, sizeof(char *));
        value[0] = tok->value;

        enum quotes *enclosure = calloc(1, sizeof(enum quotes));
        if (tok->type == TOKEN_WORD)
            enclosure[0] = Q_NONE;
        else if (tok->type == TOKEN_WORD_DOUBLE_QUOTE)
            enclosure[0] = Q_DOUBLE;
        else
            enclosure[0] = Q_SINGLE;

        (*ast)->value = value;
        (*ast)->enclosure = enclosure;

        lexer_pop(lexer);
        return PARSER_OK;
    }

    // Try redirection
    enum parser_status status_redir = parse_redirection(ast, lexer);
    if (status_redir == PARSER_ERROR)
        return handle_parser_error(status_redir, ast);

    return PARSER_OK;
}

enum parser_status parse_simple_command(struct ast **ast, struct lexer *lexer)
{
    bool first_prefix = true;
    struct lexer_token *save_tok = lexer_peek(lexer);

    // Try (prefix)*
    struct ast *cur_prefix = NULL;
    while (true)
    {
        struct ast *ast_prefix = NULL;

        // Try prefix
        enum parser_status status_prefix = parse_prefix(&ast_prefix, lexer);
        if (status_prefix == PARSER_ERROR)
        {
            lexer_go_back(lexer, save_tok);
            ast_free(ast_prefix);
            break;
        }

        if (first_prefix)
        {
            *ast = ast_prefix;
            first_prefix = false;
        }
        else
        {
            ast_prefix->left_child = cur_prefix->right_child;
            cur_prefix->right_child = ast_prefix;
        }
        cur_prefix = ast_prefix;
        save_tok = lexer_peek(lexer);
    }

    save_tok = lexer_peek(lexer);

    // Try (element)+
    bool first_element = true;
    bool first_is_command = false;
    while (true)
    {
        struct ast *ast_element = NULL;
        enum parser_status status_element = parse_element(&ast_element, lexer);
        if (status_element == PARSER_ERROR)
        {
            lexer_go_back(lexer, save_tok);
            ast_free(ast_element);
            break;
        }
        save_tok = lexer_peek(lexer);

        // Test if element is WORD and parse it as argument of the previous
        // command
        if (ast_element->type == AST_COMMAND)
        {
            struct ast *last_command = NULL;
            if ((first_element && first_prefix) || first_is_command)
            {
                first_is_command = true;
                if (cur_prefix == NULL)
                {
                    cur_prefix = ast_element;
                    first_element = false;
                    continue;
                }
                last_command = cur_prefix;
            }
            else
                last_command = cur_prefix->right_child;

            //? Merge char **value and enum quotes *enclosure from last_command
            // and ast_element
            struct string_array_with_quotes res =
                merge_values(last_command->value, ast_element->value,
                             last_command->enclosure, ast_element->enclosure);
            last_command->value = res.value;
            last_command->enclosure = res.q;
            ast_free(ast_element);
        }
        else
        {
            if ((first_element && first_prefix) || first_is_command)
            {
                *ast = ast_element;
                if (first_is_command)
                    (*ast)->left_child = cur_prefix;
            }
            else
            {
                ast_element->left_child = cur_prefix->right_child;
                cur_prefix->right_child = ast_element;
            }
            first_is_command = false;
            cur_prefix = ast_element;
        }
        first_element = false;
    }

    if (first_prefix)
    {
        //? first_element needs to be false, we are in (prefix)* (element)+
        if (first_element)
            return handle_parser_error(PARSER_ERROR, ast);
    }
    if (first_is_command)
        *ast = cur_prefix;

    return PARSER_OK;
}

enum parser_status parse_shell_command(struct ast **ast, struct lexer *lexer)
{
    enum parser_status status_command = parse_rule_if(ast, lexer);
    if (status_command == PARSER_ERROR)
        return handle_parser_error(status_command, ast);

    return PARSER_OK;
}

enum parser_status parse_command(struct ast **ast, struct lexer *lexer)
{
    struct ast *ast_simple_command = NULL;
    enum parser_status status;

    // Save of current state of lexer because of | in grammar
    struct lexer_token *save_tok = lexer_peek(lexer);

    // Try simple_command
    if ((status = parse_simple_command(&ast_simple_command, lexer))
        == PARSER_OK)
    {
        *ast = ast_simple_command;
        return status;
    }
    ast_free(ast_simple_command);

    // Go back to lexer's state before simple_command exec
    lexer_go_back(lexer, save_tok);

    // Try shell_command
    struct ast *ast_shell_command = NULL;
    if ((status = parse_shell_command(&ast_shell_command, lexer)) == PARSER_OK)
    {
        *ast = ast_shell_command;
        return status;
    }

    // Try (redirection)*
    while (true)
    {
        // TODO
        break;
    }

    ast_free(ast_shell_command);

    return PARSER_ERROR;
}
