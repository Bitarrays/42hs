#include "var_list.h"

#include <stdio.h>
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

void my_itoa(int nb, char *buff)
{
    sprintf(buff, "%d", nb);
}

int is_number(char *str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] < '0' || str[i++] > '0')
            return 0;
    }
    return 1;
}

int is_param(char *str, char *c)
{
    int i = 0;
    if (is_number(str))
        return 1;
    while (str[i] != '\0')
    {
        if (str[i++] == *c)
            return 1 && c[1] == '\0';
    }
    return 0;
}

int new_var(struct shell *sh, char **arg)
{
    struct var_stack *s = calloc(1, sizeof(struct var));
    s->var_list = NULL;
    s->next = sh->var_stack;
    sh->var_stack = s;
    int i = 0;
    char *nb = calloc(21, sizeof(char));
    int size = 1;
    char *var = calloc(size, sizeof(char));
    var[0] = '\0';
    while (arg[i])
    {
        if (i != 0)
        {
            size += strlen(arg[i]);
            var = realloc(var, size * sizeof(char));
            strcat(var, arg[i]);
        }
        my_itoa(i, nb);
        push_elt_list(sh, nb, arg[i++]);
    }
    push_int_elt_list(sh, "#", i);
    push_int_elt_list(sh, "?", 0);
    push_elt_list(sh, "@", var);
    push_elt_list(sh, "*", var);
    push_int_elt_list(sh, "$", sh->pid);
    free(nb);
    return 0;
}

int push_elt_list(struct shell *sh, char *name, char *value)
{
    int param = is_param("*@#?$", name);
    char *new_content = calloc(strlen(value) + 1, sizeof(char));
    if (!new_content)
        return 1;
    strcpy(new_content, value);
    struct var *tmp = NULL;
    if (param)
        tmp = sh->var_stack->var_list;
    else
        tmp = sh->var_list;
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
        if (param)
        {
            new->next = sh->var_stack->var_list;
            sh->var_stack->var_list = new;
        }
        else
        {
            new->next = sh->var_list;
            sh->var_list = new;
        }
    }
    return 0;
}

int push_int_elt_list(struct shell *sh, char *name, int val)
{
    char *value = calloc(21, sizeof(char));
    my_itoa(val, value);
    int param = is_param("*@#?$", name);
    char *new_content = calloc(strlen(value) + 1, sizeof(char));
    if (!new_content)
        return 1;
    strcpy(new_content, value);
    struct var *tmp = NULL;
    if (param)
        tmp = sh->var_stack->var_list;
    else
        tmp = sh->var_list;
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
        if (param)
        {
            new->next = sh->var_stack->var_list;
            sh->var_stack->var_list = new;
        }
        else
        {
            new->next = sh->var_list;
            sh->var_list = new;
        }
    }
    return 0;
}

char *find_elt_list(struct shell *sh, char *name)
{
    int param = is_param("*@#?$", name);
    struct var *tmp = NULL;
    if (param)
        tmp = sh->var_stack->var_list;
    else
        tmp = sh->var_list;
    while (tmp && strcmp(tmp->name, name))
        tmp = tmp->next;
    // printf("%s\n", tmp->value);
    if (tmp)
        return tmp->value;
    return NULL;
}

void free_list_sub(struct var *l)
{
    struct var *list = l;
    while (list)
    {
        struct var *tmp = list;
        list = list->next;
        free(tmp->name);
        free(tmp->value);
        free(tmp);
    }
}

void del_stack(struct shell *sh)
{
    struct var_stack *s = sh->var_stack;
    sh->var_stack = s->next;
    free_list_sub(s->var_list);
    free(s);
}

void free_list(struct shell *sh)
{
    while (sh->var_stack)
        del_stack(sh);
    free_list_sub(sh->var_list);
}