#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "42sh.h"

int push_elt_fun(struct shell *sh, char *name, struct ast *fun);
struct ast *find_elt_fun(struct shell *sh, char *name);
void free_fun_sub(struct shell *sh);
int del_fun_name(struct shell *sh, char *name);

#endif
