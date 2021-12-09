#include "loop_stack.h"

#include <stdlib.h>

/* if (continue) -> ast for else -> normal exec
   for check if CONTINUE -> get var list and index else -> new for add for in
   stack end of loop unstack val unset continue
 */

int push_loop(struct shell *sh, struct ast *ast, char **var, int i)
{
    struct loop_stack *new = calloc(1, sizeof(struct loop_stack));
    if (!new)
        return 1;
    new->loop = ast;
    new->var = var;
    new->index = i;
    new->next = sh->loop_stack;
    sh->loop_stack = new;
    return 0;
}

struct ast *get_ast_loop(struct shell *sh)
{
    return sh->loop_stack->loop;
}

char **get_var_loop(struct shell *sh)
{
    return sh->loop_stack->var;
}

int get_index_loop(struct shell *sh)
{
    int i = sh->loop_stack->index++;
    return i;
}

void pop_loop(struct shell *sh)
{
    struct loop_stack *tmp = sh->loop_stack;
    sh->loop_stack = tmp->next;
    free(tmp);
}