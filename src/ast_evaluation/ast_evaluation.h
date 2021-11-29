#ifndef AST_EVALUATION_H
#define AST_EVALUATION_H

#include "../parser/ast.h" // refaire plus propre
#include "ast_evaluation_tools.h"

struct var
{
    char *name;
    char **value;
    struct var *next;
};

int evaluate_ast(struct ast *ast);

#endif /* !AST_EVALUATION_H */
