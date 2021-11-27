#include "parser.h"

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
