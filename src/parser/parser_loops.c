#include "parser.h"

enum parser_status parse_rule_while(struct ast **ast, struct lexer *lexer)
{
    struct lexer_token *tok = lexer_peek(lexer);

    // Try While
    if (tok->type != TOKEN_WHILE)
        return handle_parser_error(PARSER_ERROR, ast);
    lexer_pop(lexer); // token While

    // Try compound_list
    *ast = ast_new(AST_WHILE);
    struct ast *ast_condition = ast_new(AST_LIST);
    enum parser_status status_compound_list =
        parse_compound_list(&ast_condition, lexer);
    // If status is ERROR, assignment is still legal to free everything
    (*ast)->condition = ast_condition;
    if (status_compound_list == PARSER_ERROR)
        return handle_parser_error(status_compound_list, ast);

    // Try do_group
    struct ast *ast_do = NULL;
    enum parser_status status_do_group = parse_do_group(&ast_do, lexer);
    (*ast)->left_child = ast_do;
    if (status_do_group == PARSER_ERROR)
        return handle_parser_error(status_do_group, ast);

    return PARSER_OK;
}

enum parser_status parse_rule_until(struct ast **ast, struct lexer *lexer)
{
    struct lexer_token *tok = lexer_peek(lexer);

    // Try Until
    if (tok->type != TOKEN_UNTIL)
        return handle_parser_error(PARSER_ERROR, ast);
    lexer_pop(lexer); // token Until

    // Try compound_list
    *ast = ast_new(AST_UNTIL);
    struct ast *ast_condition = ast_new(AST_LIST);
    enum parser_status status_compound_list =
        parse_compound_list(&ast_condition, lexer);
    // If status is ERROR, assignment is still legal to free everything
    (*ast)->condition = ast_condition;
    if (status_compound_list == PARSER_ERROR)
        return handle_parser_error(status_compound_list, ast);

    // Try do_group
    struct ast *ast_do = NULL;
    enum parser_status status_do_group = parse_do_group(&ast_do, lexer);
    (*ast)->left_child = ast_do;
    if (status_do_group == PARSER_ERROR)
        return handle_parser_error(status_do_group, ast);

    return PARSER_OK;
}

enum parser_status parse_do_group(struct ast **ast, struct lexer *lexer)
{
    struct lexer_token *tok = lexer_peek(lexer);

    // Try Do
    if (tok->type != TOKEN_DO)
        return handle_parser_error(PARSER_ERROR, ast);
    lexer_pop(lexer); // token While

    // Try compound_list
    struct ast *ast_body = ast_new(AST_LIST);
    enum parser_status status_compound_list =
        parse_compound_list(&ast_body, lexer);

    *ast = ast_body;
    if (status_compound_list == PARSER_ERROR)
        return handle_parser_error(status_compound_list, ast);

    // Try Done
    tok = lexer_peek(lexer);
    if (tok->type != TOKEN_DONE)
        return handle_parser_error(PARSER_ERROR, ast);
    lexer_pop(lexer); // token Done

    return PARSER_OK;
}
