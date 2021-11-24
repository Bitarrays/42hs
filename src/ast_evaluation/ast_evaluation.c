#include "ast_evaluation.h"

#include <stdio.h>
#include <string.h>

#include "../builtins/builtins.h"

int evaluate_ast(struct ast *ast)
{
    if (!ast)
        return 0;

    if (ast->type == AST_IF)
    {
        if (!evaluate_ast(ast->condition))
            return evaluate_ast(ast->left_child);
        else
            return evaluate_ast(ast->right_child);
    }
    /*    else if (ast->type == AST_FOR)
        {

            evaluate_ast(ast->condition);

            evaluate_ast(ast->left_child);

        }*/
    else if (ast->type == AST_WHILE)
    {
        while (!evaluate_ast(ast->condition))
            evaluate_ast(ast->left_child);
        return evaluate_ast(ast->left_child);
    }
    else if (ast->type == AST_AND)
    {
        return evaluate_ast(ast->left_child) && evaluate_ast(ast->right_child);
    }
    else if (ast->type == AST_OR)
    {
        return evaluate_ast(ast->left_child) || evaluate_ast(ast->right_child);
    }
    /*else if (ast->type == AST_REDIR)
    {

        evaluate_ast(ast->right_child); // talk about that with Fisch


        evaluate_ast(ast->left_child);
    }*/
    else if (ast->type == AST_COMMAND)
    {
        if (is_builtin(*(ast->value)))
            return find_command(ast->value);
        else
            return call_exec(ast->value);
    }
    else if (ast->type == AST_LIST)
    {
        evaluate_ast(ast->right_child);
        return evaluate_ast(ast->left_child);
    }
    else
    {
        printf("Not implemented yet\n");
    }

    return 0;
}