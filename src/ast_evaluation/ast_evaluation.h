#ifndef AST_EVALUATION_H
#define AST_EVALUATION_H

#include "../parser/ast.h" // refaire plus propre
#include "../42sh.h"
#include "ast_evaluation_tools.h"

int evaluate_ast(struct ast *ast);

#endif /* !AST_EVALUATION_H */
