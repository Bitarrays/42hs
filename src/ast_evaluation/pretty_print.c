#include "ast_evaluation_tools.h"

void pretty_print(struct ast *ast)
{
    if (!ast)
    {
        if (ast->type == AST_IF || ast->type == AST_FOR || ast->type == AST_WHILE)
        {
            if (ast->type == AST_IF)
            {
                printf("if {");
                pretty_print(ast->condition);
                printf(" }; then { ");
                pretty_print(ast->left_child);
                printf(" }");
                if (ast->right_child)
                {
                    printf(" else { ");
                    pretty_print(ast->right_child);
                    printf(" }");
                }
            }
            else if (ast->type == AST_FOR)
            {
                printf("for {");
                pretty_print(ast->condition);
                printf(" }; do { ");
                pretty_print(ast->left_child);
                printf(" }");
            }
            else if (ast->type == AST_WHILE)
            {
                printf("while {");
                pretty_print(ast->condition);
                printf(" }; do { ");
                pretty_print(ast->left_child);
                printf(" }");
            }
        }
        else if (ast->type == AST_AND)
        {
            printf("{ ");
            pretty_print(ast->left_child);
            printf(" } && { ");
            pretty_print(ast->right_child);
            printf(" }");
        }
        else if (ast->type == AST_AND)
        {
            printf("{ ");
            pretty_print(ast->left_child);
            printf(" } OR { ");
            pretty_print(ast->right_child);
            printf(" }");
        }
        else if (ast->type == AST_REDIR)
        {
            printf("redir \"");
            pretty_print(ast->right_child); // talk about that with Fisch
            pretty_print(ast->value);
            printf(" ");
            pretty_print(ast->left_child);
        }
        else if (ast->type == AST_COMMAND)
        {
            printf(ast->value);
        }
    }
    else
    {
        printf("NULL");
    }
    printf("\n");
}