#include <stdlib.h>
#include <string.h>

#include "ast_evaluation.c"

struct var *init_list(void)
{
    struct var *new = calloc(1, sizeof(struct var));
    if (!new)
        return NULL;
    new->name = NULL;
    new->value = NULL;
    new->next = NULL;
}

int add_elt_list(struct var *list, char *name, char *value)
{
    struct var *new = calloc(1, sizeof(struct var));
    if (!new)
        return 1;
    new->name = calloc(strlen(name) + 1, sizeof(char));
    if (!new->name)
    {
        free(new);
        return 1;
    }
    new->value = calloc(strlen(value) + 1, sizeof(char));
    if (!new->value)
    {
        free(new->value);
        free(new);
        return 1;
    }
    new->next = list->next;
    list->next = new;
    return 0;
}

char *find_elt_list(struct var *list, char *name)
{
    struct var *tmp = list->next;
    while (tmp && strcmp(tmp->name, name))
        tmp->next = tmp;
    if (tmp)
        return tmp->value;
    return NULL;
}

void free_list(struct var *list)
{
    while (list)
    {
        struct var *tmp = list;
        list = list->next;
        free(tmp->name);
        free(tmp->value);
        free(tmp);
    }
}