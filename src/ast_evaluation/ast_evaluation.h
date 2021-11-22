#infndef AST_EVALUATION_H
#define AST_EVALUATION_H

#include "../parser/ast.h" // refaire plus propre
#include "ast_evaluation_tools.h"
// include builtin
int evaluate_ast(struct lexer *ast);

#endif /* !AST_EVALUATION_H */