#include <stdlib.h>
#include <string.h>

#include "../42sh.h"
#include "var_list.h"

/*struct var *init_list(void)
{
    struct var *new = calloc(1, sizeof(struct var));
    if (!new)
        return NULL;
    new->name = NULL;
    new->value = NULL;
    new->next = NULL;
}*/

int add_elt_list(struct shell *sh, char *name, char *value)
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
    /*int nb_elt = 0;
    while (value[nb_elt] != NULL)
        nb_elt++;
    new->value = calloc(nb_elt + 1, sizeof(char *));
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
        strcpy(new->value[i], value[i]);
        i++;
    }
    new->value[i] = NULL;*/
    new->next = sh->var_list;
    sh->var_list = new;
    return 0;
}

int change_elt_list(struct shell *sh, char *name, char *value)
{
    /*char **new_content = NULL;
    int nb_elt = 0;
    while (value[nb_elt] != NULL)
        nb_elt++;
    new_content= calloc(nb_elt + 1, sizeof(char *));
    int i = 0;
    while (value[i] != NULL)
    {
        new_content[i] = calloc(strlen(value[i]) + 1, sizeof(char));
        if (!new_content)
            return 1;
        strcpy(new_content[i], value[i]);
        i++;
    }
    new_content[i] = NULL;
    struct var *tmp = sh->var_list;
    while (tmp && strcmp(tmp->name, name))
        tmp->next = tmp;
    if (tmp)
    {
        int j = 0;
        while (tmp->value[j])
            free(tmp->value[j++]);
        free(tmp->value);
    }*/
    char *new_content = calloc(strlen(value) + 1, sizeof(char));
    if (!new_content)
        return 1;
    strcpy(new_content, value);
    struct var *tmp = sh->var_list;
    while (tmp && strcmp(tmp->name, name))
        tmp->next = tmp;
    if (tmp)
    {
        free(tmp->value);
        tmp->value = new_content;
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