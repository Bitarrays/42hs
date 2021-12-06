#include "var_list.h"

#include <stdlib.h>
#include <string.h>

#include "../42sh.h"

/*struct var *init_list(void)
{
    struct var *new = calloc(1, sizeof(struct var));
    if (!new)
        return NULL;
    new->name = NULL;
    new->value = NULL;
    new->next = NULL;
}*/

/*int add_elt_list(struct shell *sh, char *name, char *value)
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
    new->value = calloc(strlen(value) + 1, sizeof(char));
    if (!new->value)
    {
        free(new->name);
        free(new);
        return 1;
    }
    strcpy(new->value, value);
    new->next = sh->var_list;
    sh->var_list = new;
    return 0;
}*/

int push_elt_list(struct shell *sh, char *name, char *value)
{
    char *new_content = calloc(strlen(value) + 1, sizeof(char));
    if (!new_content)
        return 1;
    strcpy(new_content, value);
    struct var *tmp = sh->var_list;
    while (tmp && strcmp(tmp->name, name))
        tmp = tmp->next;
    if (tmp)
    {
        free(tmp->value);
        tmp->value = new_content;
    }
    else
    {
        struct var *new = calloc(1, sizeof(struct var));
        if (!new)
            return 1;
        new->name = calloc(strlen(name) + 1, sizeof(char));
        if (!new->name)
        {
            free(new);
            free(new_content);
            return 1;
        }
        strcpy(new->name, name);
        new->value = new_content;
        new->next = sh->var_list;
        sh->var_list = new;
    }
    return 0;
}

char *find_elt_list(struct shell *sh, char *name)
{
    struct var *tmp = sh->var_list;
    while (tmp && strcmp(tmp->name, name))
        tmp = tmp->next;
    if (tmp)
        return tmp->value;
    return NULL;
}

void free_list(struct shell *sh)
{
    struct var *list = sh->var_list;
    while (list)
    {
        struct var *tmp = list;
        list = list->next;
        free(tmp->name);
        free(tmp->value);
        free(tmp);
    }
}