#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast_evaluation_tools.h"

int is_in(char **condition)
{
    while ((*condition)[0] != '\0')
    {
        if (!strcmp(*condition, "in"))
            return 1;
        condition++;
    }
    return 0;
}

int expand_s(char **elt, char *s, enum quotes type)
{
    char *new = calloc(strlen(s) + 1, sizeof(char));
    if (!new)
        return 0;
    strcpy(new, s);
    if (type == Q_NONE)
    {
        int i = 0;
        int i_new = 0;
        while (s[i] != '\0')
        {
            if (s[i] == '\\')
                i++;
            new[i_new++] = s[i++];
        }
        new[i_new] = '\0';
    }
    /*else if (type == Q_DOUBLE)
    {
       return ;
    }*/
    *elt = new;
    return 1;
}

int array_len(char **arr)
{
    int i = 0;
    while (arr[i] != NULL)
        i++;
    return i;
}

char **expand(struct ast *ast)
{
    char **new = calloc(array_len(ast->value) + 1, sizeof(char *));
    if (!new)
        return NULL;
    int ret_val = 1;
    int i = 0;
    while (ast->value[i] != NULL && ret_val)
    {
        ret_val = expand_s(new + i, ast->value[i], ast->enclosure[i]);
        i++;
    }
    new[i] = NULL;
    if (!ret_val)
        return NULL;
    return new;
}