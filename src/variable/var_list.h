#ifndef VAR_LIST_H
#define VAR_LIST_H

#include "../42sh.h"

int add_elt_list(struct shell *sh, char *name, char *value);
char *find_elt_list(struct shell *sh, char *name);
void free_list(struct shell *sh);
int change_elt_list(struct shell *sh, char *name, char *value);

#endif /* !VAR_LIST_H */