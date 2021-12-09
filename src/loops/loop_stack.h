#ifndef LOOP_STACK_H
#define LOOP_STACK_H

#include "../42sh.h"
#include "ast.h"

int push_loop(struct shell *sh, struct ast *ast);
struct ast *get_ast_loop(struct shell *sh);
void pop_loop(struct shell *sh);

#endif /* !LOOP_STACK_H */