#include "parser.h"

#include <err.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

struct string_array_with_quotes
{
    char **value;
    enum quotes *q;
};

void pretty_print(struct ast *ast);
int evaluate_ast(struct ast *ast);

static enum parser_status handle_parser_error(enum parser_status status,
                                              struct ast **res)
{
    ast_free(*res);
    *res = NULL;
    return status;
}

static int display_parser_error(struct ast **res)
{
    warnx("Parser: unexpected token");
    ast_free(*res);
    *res = NULL;
    return 2;
}

static struct string_array_with_quotes merge_values(char **values_1,
                                                    char **values_2,
                                                    enum quotes *q_1,
                                                    enum quotes *q_2)
{
    int length_1 = 0;
    while (values_1[length_1] != NULL)
        length_1++;

    int length_2 = 0;
    while (values_2[length_2] != NULL)
        length_2++;

    values_1 = realloc(values_1, (length_1 + 1 + length_2) * sizeof(char *));
    q_1 = realloc(q_1, (length_1 + length_2) * sizeof(enum quotes));
    for (int i = length_1; i < length_2 + length_1; i++)
    {
        values_1[i] = values_2[i - length_1];
        q_1[i] = q_2[i - length_1];
    }
    values_1[length_1 + length_2] = NULL;

    struct string_array_with_quotes res = { values_1, q_1 };
    return res;
}

static enum parser_status add_eof_node(struct ast **ast)
{
    struct ast *cur = *ast;
    while (cur && cur->left_child && cur->type == AST_LIST)
    {
        cur = cur->left_child;
    }
    if (!cur || cur->type != AST_LIST)
        return PARSER_ERROR;

    cur->left_child = ast_new(AST_EOF);
    return PARSER_OK;
}

int parse_input(char *input)
{
    struct lexer *lex = lexer_create(input);
    lexer_build(lex);

    struct ast *ast = ast_new(AST_LIST);

    // Try EOF
    struct lexer_token *end = lexer_peek(lex);
    if (end->type == TOKEN_EOF || end->type == TOKEN_NEWLINE)
    {
        ast_free(ast);
        lexer_free(lex);
        return PARSER_OK;
    }

    // Try compound_list EOF
    if (parse_compound_list(&ast, lex) == PARSER_OK)
    {
        struct lexer_token *next = lexer_peek(lex);
        if (next->type == TOKEN_EOF || next->type == TOKEN_NEWLINE)
        {
            if (add_eof_node(&ast) == PARSER_ERROR)
                return display_parser_error(&ast);

            if (shell->pretty_print)
                pretty_print(ast);
            int res_eval = evaluate_ast(ast);

            ast_free(ast);
            lexer_free(lex);
            return res_eval;
        }
    }

    lexer_free(lex);
    return display_parser_error(&ast);
}

enum parser_status parse_compound_list(struct ast **ast, struct lexer *lexer)
{
    // Try and_or
    enum parser_status status_and_or =
        parse_and_or(&(*ast)->right_child, lexer);
    if (status_and_or == PARSER_ERROR)
        return handle_parser_error(status_and_or, ast);

    // Try (';' and_or)*
    struct ast *cur_list_node = *ast;
    while (true)
    {
        struct lexer_token *save_tok = lexer_peek(lexer);
        if (save_tok->type != TOKEN_SEMICOLON)
            break;
        lexer_pop(lexer);

        struct ast *new_list = ast_new(AST_LIST);
        enum parser_status status = parse_and_or(&new_list->right_child, lexer);
        if (status == PARSER_ERROR)
        {
            ast_free(new_list);
            lexer_go_back(lexer, save_tok);
            break;
        }

        cur_list_node->left_child = new_list;
        cur_list_node = cur_list_node->left_child;
    }

    // Try [';'] and skip it if present
    struct lexer_token *tok = lexer_peek(lexer);
    if (tok->type == TOKEN_SEMICOLON)
        lexer_pop(lexer);

    return PARSER_OK;
}

static enum parser_status parse_redirection(struct ast **ast,
                                            struct lexer *lexer)
{
    if (!ast || !lexer)
        return PARSER_ERROR;
    return PARSER_ERROR;
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

    // Try (prefix)*
    struct ast *cur_prefix = NULL;
    while (true)
    {
        struct ast *ast_prefix = NULL;

        // Try prefix
        enum parser_status status_prefix = parse_prefix(&ast_prefix, lexer);
        if (status_prefix == PARSER_ERROR)
        {
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
    }

    // Try (element)+
    bool first_element = true;
    bool first_is_command = false;
    while (true)
    {
        struct ast *ast_element = NULL;
        enum parser_status status_element = parse_element(&ast_element, lexer);
        if (status_element == PARSER_ERROR)
        {
            ast_free(ast_element);
            break;
        }

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
            //and ast_element
            struct string_array_with_quotes res =
                merge_values(last_command->value, ast_element->value,
                             last_command->enclosure, ast_element->enclosure);
            last_command->value = res.value;
            last_command->enclosure = res.q;
            ast_free(ast_element);
        }
        else
        {
            first_is_command = false;
            if (first_prefix && first_element)
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

enum parser_status parse_pipeline(struct ast **ast, struct lexer *lexer)
{
    struct lexer_token *tok = lexer_peek(lexer);
    // Try not
    bool not = false;
    if (tok->type == TOKEN_NOT)
    {
        not = true;
        *ast = ast_new(AST_NOT);
        lexer_pop(lexer);
    }

    // Try command
    struct ast *last_command = NULL;
    enum parser_status status_command = parse_command(&last_command, lexer);
    if (status_command == PARSER_ERROR)
    {
        ast_free(*ast);
        return handle_parser_error(status_command, &last_command);
    }

    // Try ('|' ('\n')* command)*
    struct ast *cur_pipe = NULL;
    bool first = true;
    while (true)
    {
        // Try |
        tok = lexer_peek(lexer);
        if (tok->type != TOKEN_PIPE)
            break;
        lexer_pop(lexer); // token |

        // Try ('\n')*
        while ((tok = lexer_peek(lexer))->type == TOKEN_NEWLINE)
            lexer_pop(lexer); // token \n

        // Try command
        struct ast *new_command;
        status_command = parse_command(&new_command, lexer);
        if (status_command == PARSER_ERROR)
        {
            ast_free(new_command);
            ast_free(last_command);
            return handle_parser_error(status_command, ast);
        }

        struct ast *ast_pipe = ast_new(AST_PIPE);
        //* Create new pipe and add command found before while loop in left
        //child
        if (first)
        {
            first = false;
            ast_pipe->left_child = last_command;
            if (not )
                (*ast)->left_child = ast_pipe;
            else
                *ast = ast_pipe;
        }
        else
        {
            cur_pipe->right_child = ast_pipe;
            ast_pipe->left_child = last_command;
        }
        cur_pipe = ast_pipe;
        last_command = new_command;
    }
    if (cur_pipe)
        cur_pipe->right_child = last_command;

    if (*ast == NULL)
        *ast = last_command;

    return PARSER_OK;
}

enum parser_status parse_and_or(struct ast **ast, struct lexer *lexer)
{
    // Try pipeline
    enum parser_status status_pipeline = parse_pipeline(ast, lexer);
    if (status_pipeline == PARSER_ERROR)
        return handle_parser_error(status_pipeline, ast);

    return PARSER_OK;
}

enum parser_status parse_rule_if(struct ast **ast, struct lexer *lexer)
{
    // Check If
    struct lexer_token *tok = lexer_peek(lexer);
    if (tok->type != TOKEN_IF)
        return handle_parser_error(PARSER_ERROR, ast);
    lexer_pop(lexer); // token IF

    *ast = ast_new(AST_IF);

    // Check compound_list (condition)
    struct ast *ast_condition = ast_new(AST_LIST);
    enum parser_status status_compound_list =
        parse_compound_list(&ast_condition, lexer);
    // If status is ERROR, assignment is still legal to free everything
    (*ast)->condition = ast_condition;
    if (status_compound_list == PARSER_ERROR)
        return handle_parser_error(status_compound_list, ast);

    // Check Then
    tok = lexer_peek(lexer);
    if (tok->type != TOKEN_THEN)
        return handle_parser_error(PARSER_ERROR, ast);
    lexer_pop(lexer); // token THEN

    // Check compound_list (true block)
    struct ast *ast_true_block = ast_new(AST_LIST);
    status_compound_list = parse_compound_list(&ast_true_block, lexer);
    (*ast)->left_child = ast_true_block;
    if (status_compound_list == PARSER_ERROR)
        return handle_parser_error(status_compound_list, ast);

    // Check First(else_clause) = {Else, Elif}
    tok = lexer_peek(lexer);
    if (tok->type == TOKEN_ELSE || tok->type == TOKEN_ELIF)
    {
        // Check else_clause
        struct ast *ast_false_block = NULL;
        status_compound_list = parse_else_clause(&ast_false_block, lexer);
        (*ast)->right_child = ast_false_block;
        if (status_compound_list == PARSER_ERROR)
            return handle_parser_error(status_compound_list, ast);
    }

    // Check Fi
    tok = lexer_peek(lexer);
    if (tok->type != TOKEN_FI)
        return handle_parser_error(PARSER_ERROR, ast);
    lexer_pop(lexer); // token FI

    return PARSER_OK;
}

enum parser_status parse_else_clause(struct ast **ast, struct lexer *lexer)
{
    // Double check First(else_clause), should always be correct but safety
    // first
    struct lexer_token *tok = lexer_peek(lexer);
    enum parser_status status = PARSER_OK;
    if (tok->type == TOKEN_ELSE)
    {
        lexer_pop(lexer); // token ELSE

        // Check compound_list
        *ast = ast_new(AST_LIST);
        status = parse_compound_list(ast, lexer);
        if (status == PARSER_ERROR)
            return handle_parser_error(status, ast);
    }
    else if (tok->type == TOKEN_ELIF)
    {
        lexer_pop(lexer); // token ELIF
        (*ast) = ast_new(AST_IF);

        // Check compound_list
        struct ast *ast_elif_condition = ast_new(AST_LIST);
        status = parse_compound_list(&ast_elif_condition, lexer);
        (*ast)->condition = ast_elif_condition;
        if (status == PARSER_ERROR)
            return handle_parser_error(status, ast);

        // Check Then
        tok = lexer_peek(lexer);
        if (tok->type != TOKEN_THEN)
            return handle_parser_error(PARSER_ERROR, ast);
        lexer_pop(lexer); // token THEN

        // Check compound_list (true block)
        struct ast *ast_true_block = ast_new(AST_LIST);
        status = parse_compound_list(&ast_true_block, lexer);
        (*ast)->left_child = ast_true_block;
        if (status == PARSER_ERROR)
            return handle_parser_error(status, ast);

        // Check First(else_clause) = {Else, Elif}
        tok = lexer_peek(lexer);
        if (tok->type == TOKEN_ELSE || tok->type == TOKEN_ELIF)
        {
            struct ast *ast_false_block = NULL;
            status = parse_else_clause(&ast_false_block, lexer);
            (*ast)->right_child = ast_false_block;
            if (status == PARSER_ERROR)
                return handle_parser_error(status, ast);
        }
    }
    else
        return handle_parser_error(PARSER_ERROR, ast);

    return PARSER_OK;
}
