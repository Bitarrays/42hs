#include "parser.h"

#include <err.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

static enum parser_status handle_parser_error(enum parser_status status,
                                              struct ast **res)
{
    warnx("Parser: unexpected token");
    ast_free(*res);
    *res = NULL;
    return status;
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
            // lexer_free(lex);

            // TODO: call evaluation function with ast

            // printf("ast type: %d\n", ast->type);
            // if (ast->right_child)
            //     printf("right child: %d\n", ast->right_child->type);
            // if (ast->left_child)
            // {
            //     printf("left child: %d\n", ast->left_child->type);
            //     if (ast->left_child->right_child)
            //         printf("left->right child: %d\n",
            //         ast->left_child->right_child->type);
            // }

            ast_free(ast);
            return PARSER_OK;
        }
    }

    // Try EOF
    if (lexer_peek(lex)->type == TOKEN_EOF)
    {
        lexer_free(lex);

        // TODO: call evaluation function with ast

        ast_free(ast);
        return PARSER_OK;
    }

    lexer_free(lex);
    return handle_parser_error(PARSER_ERROR, &ast);
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
        struct lexer_token *tok = lexer_peek(lexer);
        if (tok->type != TOKEN_SEMICOLON)
            break;
        lexer_token_free(lexer_pop(lexer));

        struct ast *new_list = ast_new(AST_LIST);
        enum parser_status status = parse_and_or(&new_list->right_child, lexer);
        if (status == PARSER_ERROR)
            break;

        cur_list_node->left_child = new_list;
        cur_list_node = cur_list_node->left_child;
    }

    // Try [';'] and skip it if present
    struct lexer_token *tok = lexer_peek(lexer);
    if (tok->type == TOKEN_SEMICOLON)
        lexer_token_free(lexer_pop(lexer));

    return PARSER_OK;
}

enum parser_status parse_simple_command(struct ast **ast, struct lexer *lexer)
{
    struct lexer_token *tok = lexer_peek(lexer);
    bool first = true;
    char *value = calloc(1, sizeof(char));
    size_t value_len = 0;

    // Try WORD*
    while (tok->type == TOKEN_WORD)
    {
        if (!first)
        {
            value_len++;
            value = realloc(value, (value_len + 1) * sizeof(char));
            value[value_len - 1] = ' ';
            value[value_len] = '\0';
        }
        else
            first = false;

        value_len += strlen(tok->value);
        value = realloc(value, (value_len + 1) * sizeof(char));
        value = strcat(value, tok->value);
        value[value_len] = '\0';

        lexer_token_free(lexer_pop(lexer));
        tok = lexer_peek(lexer);
    }

    *ast = ast_new(AST_COMMAND);
    (*ast)->value = value;
    printf("Command: %s\n", value);

    return PARSER_OK;
}

enum parser_status parse_shell_command(struct ast **ast, struct lexer *lexer)
{
    return PARSER_OK;
}

enum parser_status parse_command(struct ast **ast, struct lexer *lexer)
{
    struct ast *ast_simple_command;
    enum parser_status status;

    // Try simple_command
    if ((status = parse_simple_command(&ast_simple_command, lexer))
        == PARSER_OK)
    {
        *ast = ast_simple_command;
        return status;
    }
    ast_free(ast_simple_command);

    // Try shell_command
    // struct ast *ast_shell_command;
    // if ((status = parse_shell_command(&ast_shell_command, lexer)) ==
    // PARSER_OK)
    // {
    //     *ast = ast_shell_command;
    //     return status;
    // }
    // ast_free(ast_shell_command);

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
    return PARSER_OK;
}

enum parser_status parse_else_clause(struct ast **ast, struct lexer *lexer)
{
    return PARSER_OK;
}
