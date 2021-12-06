#ifndef AST_EVALUATION_H
#define AST_EVALUATION_H

#include "../42sh.h"
#include "../parser/ast.h" // refaire plus propre
#include "ast_evaluation_tools.h"
#include "var_list.h"

int evaluate_ast(struct ast *ast);

#endif /* !AST_EVALUATION_H */
