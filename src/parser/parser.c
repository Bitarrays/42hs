#include "parser.h"

#include <err.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

void pretty_print(struct ast *ast);
int evaluate_ast(struct ast *ast);

static enum parser_status handle_parser_error(enum parser_status status,
                                              struct ast **res)
{
    ast_free(*res);
    *res = NULL;
    return status;
}

static enum parser_status display_parser_error(enum parser_status status,
                                               struct ast **res)
{
    warnx("Parser: unexpected token\n");
    ast_free(*res);
    *res = NULL;
    return status;
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

enum parser_status parse_input(char *input)
{
    struct lexer *lex = lexer_create(input);
    lexer_build(lex);

    struct ast *ast = ast_new(AST_LIST);

    // Try compound_list EOF
    if (parse_compound_list(&ast, lex) == PARSER_OK)
    {
        if (lexer_peek(lex)->type == TOKEN_EOF)
        {
            if (add_eof_node(&ast) == PARSER_ERROR)
                return display_parser_error(PARSER_ERROR, &ast);

            pretty_print(ast);
            printf("\n");
            evaluate_ast(ast);

            ast_free(ast);
            lexer_free(lex);
            return PARSER_OK;
        }
    }

    // Try EOF
    if (lexer_peek(lex)->type == TOKEN_EOF)
    {
        ast_free(ast);
        lexer_free(lex);
        return PARSER_OK;
    }

    lexer_free(lex);
    return display_parser_error(PARSER_ERROR, &ast);
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

enum parser_status parse_simple_command(struct ast **ast, struct lexer *lexer)
{
    struct lexer_token *tok = lexer_peek(lexer);
    bool first = true;
    char **args_exec = NULL;
    enum quotes *enclosure = NULL;
    size_t value_len = 0;

    // Try WORD+
    while (tok->type == TOKEN_WORD || tok->type == TOKEN_WORD_SINGLE_QUOTE
           || tok->type == TOKEN_WORD_DOUBLE_QUOTE)
    {
        if (first)
            first = false;

        value_len++;
        args_exec = realloc(args_exec, (value_len + 1) * sizeof(char *));
        args_exec[value_len] = NULL;
        args_exec[value_len - 1] = tok->value;

        enclosure = realloc(enclosure, value_len * sizeof(enum quotes));
        if (tok->type == TOKEN_WORD)
            enclosure[value_len - 1] = Q_NONE;
        else if (tok->type == TOKEN_WORD_SINGLE_QUOTE)
            enclosure[value_len - 1] = Q_SINGLE;
        else if (tok->type == TOKEN_WORD_DOUBLE_QUOTE)
            enclosure[value_len - 1] = Q_DOUBLE;

        lexer_pop(lexer);
        tok = lexer_peek(lexer);
    }

    if (!first)
    {
        *ast = ast_new(AST_COMMAND);
        (*ast)->value = args_exec;
        (*ast)->enclosure = enclosure;
    }

    return first ? PARSER_ERROR : PARSER_OK;
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
    ast_free(ast_shell_command);

    return PARSER_ERROR;
}

enum parser_status parse_pipeline(struct ast **ast, struct lexer *lexer)
{
    // Try command
    enum parser_status status_command = parse_command(ast, lexer);
    if (status_command == PARSER_ERROR)
        return handle_parser_error(status_command, ast);

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
