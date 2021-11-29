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
    /*else if (ast->type == AST_FOR)
    {
        if (!ast->condition->left_child
            && is_in(ast->condition->right_child->value))
            return 0; // add expend var

        while (!evaluate_ast(ast->condition))
            evaluate_ast(ast->left_child);
        return evaluate_ast(ast->right_child);
    }*/
    else if (ast->type == AST_WHILE)
    {
        int ret = 0;
        while (!evaluate_ast(ast->condition))
            ret = evaluate_ast(ast->left_child);
        return ret;
    }
    else if (ast->type == AST_UNTIL)
    {
        int ret = 0;
        while (evaluate_ast(ast->condition))
            ret = evaluate_ast(ast->left_child);
        return ret;
    }
    else if (ast->type == AST_AND)
    {
        return !evaluate_ast(ast->left_child)
            && !evaluate_ast(ast->right_child);
    }
    else if (ast->type == AST_OR)
    {
        return !evaluate_ast(ast->left_child)
            || !evaluate_ast(ast->right_child);
    }
    /*else if (ast->type == AST_REDIR)
    {

        evaluate_ast(ast->right_child); // talk about that with Fisch


        evaluate_ast(ast->left_child);
    }*/
    else if (ast->type == AST_COMMAND)
    {
        char **val = expand(ast);
        if (!val)
            return 1;
        int res;
        if (is_builtin(*(val)))
            res = find_command(val);
        else
            res = call_exec(val);
        char *tmp = val[0];
        int pos = 0;
        while (tmp)
        {
            free(tmp);
            tmp = val[++pos];
        }
        free(val);
        return res;
    }
    else if (ast->type == AST_NOT)
        return !evaluate_ast(ast->left_child);
    else if (ast->type == AST_LIST)
    {
        int r = evaluate_ast(ast->right_child);
        if (ast->left_child && ast->left_child->type != AST_EOF)
            return evaluate_ast(ast->left_child);
        return r;
    }
    else
    {
        printf("Not implemented yet\n");
    }

    return 0;
}
