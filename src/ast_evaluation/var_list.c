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

int add_elt_list(struct var *list, char *name, char **value)
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
    strcpy(new->name, name);
    int nb_elt = 0;
    while (value[nb_elt] != NULL)
        nb_elt++;
    new->value = calloc(strlen(nb_elt), sizeof(char *));
    int i = 0;
    while (value[i] != NULL)
    {
        new->value[i] = calloc(strlen(value[i]) + 1, sizeof(char));
        if (!new->value)
        {
            free(new->name);
            free(new);
            return 1;
        }
        strcmp(new->value[i], value[i]);
        i++;
    }
    value[i] = NULL;
    new->next = list->next;
    list->next = new;
    return 0;
}

char **find_elt_list(struct var *list, char *name)
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
        int i = 0;
        while (tmp->value[i] != NULL)
            free(tmp->value[i++]);
        free(tmp->value);
        free(tmp);
    }
}