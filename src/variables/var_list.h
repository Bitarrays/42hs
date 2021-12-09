#ifndef VAR_LIST_H
#define VAR_LIST_H

#include "../42sh.h"

int push_elt_list(struct shell *sh, char *name, char *value);
char *find_elt_list(struct shell *sh, char *name);
void free_list(struct shell *sh);
int push_int_elt_list(struct shell *sh, char *name, int val);
void del_stack(struct shell *sh);
int new_var(struct shell *sh, char **arg);
// int change_elt_list(struct shell *sh, char *name, char *value);

#endif /* !VAR_LIST_H */
