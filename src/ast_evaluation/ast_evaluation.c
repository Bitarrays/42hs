#include <stdio.h>
#include <string.h>

#include "ast_evaluation.h"

int evaluate_ast(struct ast *ast)
{
    if (!ast)
        return 0;
    
    if (ast->type == AST_COMMAND)
    {
        call_builtin(ast->value); // check return value
    }
    else
    {
        printf("Not implemented yet\n");
    }

    (void) ast;

    return 0;
}