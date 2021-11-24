#include "ast_evaluation.h"

#include <stdio.h>
#include <string.h>

int evaluate_ast(struct ast *ast)
{
    if (!ast)
        return 0;

    if (ast->type == AST_LIST)
    {
        call_builtin(ast->right_child->value); // check return value
    }
    else
    {
        printf("Not implemented yet\n");
    }

    (void)ast;

    return 0;
}