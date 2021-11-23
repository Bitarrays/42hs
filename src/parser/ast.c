#include "ast.h"

#include <stdlib.h>

struct ast *ast_new(enum ast_type type)
{
    struct ast *new = calloc(1, sizeof(struct ast));
    new->type = type;
    return new;
}

void ast_free(struct ast *ast)
{
    if (ast == NULL)
        return;

    ast_free(ast->left_child);
    ast->left_child = NULL;

    ast_free(ast->right_child);
    ast->right_child = NULL;

    ast_free(ast->condition);
    ast->condition = NULL;

    free(ast);
}