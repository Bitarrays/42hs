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

/*int expend_var(char *s, char *new, int *size, int *i)
{
    int bracket = s[++(*i)] == '{';
    int size_var = 0;
    if (bracket)
        (*i)++;
    while (s[*i] != '\0' && s[*i] != ' ' && s[*i] != '\t' && (bracket))
    {
        (*i)++;
        size_var++;
    }
    *size += size_var;
    char *tmp = realloc(new, (*size + 1) * sizeof(char *));
    strncat(new, )
}*/

int expand_s(char **elt, char *s, enum quotes type)
{
    int size = strlen(s);
    char *new = calloc(size + 1, sizeof(char));
    if (!new)
        return 0;
    strcpy(new, s);
    if (type == Q_NONE)
    {
        int i = 0;
        int i_new = 0;
        int escaped = 0;
        while (s[i] != '\0')
        {
            if (escaped)
                escaped = 0;
            else if (s[i] == '$')
            {
                int bracket = s[++i] == '{';
                int offset = 1;
                if (bracket)
                {
                    offset++;
                    i++;
                }
                int size_var = 0;
                while (s[i] != '\0' && s[i] != ' ' && s[i] != '\t'
                       && s[i] != '$' && (!bracket || (bracket && s[i] != '}')))
                {
                    i++;
                    size_var++;
                }
                if (bracket)
                    i++;
                size += size_var;
                char *name = calloc(size_var + 1, sizeof(char));
                strncpy(name, new + i_new + offset, size_var);
                name[size_var] = '\0';
                if (!find_elt_list(shell, name))
                    return 0;
                int new_size = strlen(find_elt_list(shell, name));
                char *tmp = realloc(new, (new_size + 1) * sizeof(char *));
                if (!tmp)
                    return 0;
                new = tmp;
                strcpy(new + i_new, find_elt_list(shell, name));
                i_new += new_size;
                free(name);
                if (s[i] == '\0')
                    break;
                if (s[i] == '$')
                    continue;
            }
            else if (s[i] == '\\')
            {
                i++;
                escaped = 1;
            }
            new[i_new++] = s[i++];
        }
        new[i_new] = '\0';
    }
    else if (type == Q_DOUBLE)
    {
        int i = 0;
        int i_new = 0;
        int escaped = 0;
        while (s[i] != '\0')
        {
            if (escaped)
                escaped = 0;
            else if (s[i] == '\\')
            {
                i++;
                if (s[i] == 'n')
                {
                    i++;
                    new[i_new++] = '\n';
                }
                else if (s[i] == '\"')
                {
                    i++;
                    new[i_new++] = '\"';
                }
                else if (s[i] == '\'')
                {
                    printf("->%d\n", s[i]);
                    i++;
                    new[i_new++] = '\'';
                }
                else
                    escaped = 1;
            }
            new[i_new++] = s[i++];
        }
        new[i_new] = '\0';
    }
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

char **expand(char **arg, enum quotes *enclosure)
{
    char **new = calloc(array_len(arg) + 1, sizeof(char *));
    if (!new)
        return NULL;
    int ret_val = 1;
    int i = 0;
    while (arg[i] != NULL && ret_val)
    {
        ret_val = expand_s(new + i, arg[i], enclosure[i]);
        i++;
    }
    new[i] = NULL;
    if (!ret_val)
        return NULL;
    return new;
}

int str_in(char *s, char c)
{
    int i = 0;
    while (s[i] != '\0')
    {
        if (s[i++] == c)
            return 1;
    }
    return 0;
}

char *merge_arg(char **arg)
{
    char *s = calloc(1, sizeof(char));
    s[0] = '\0';
    int size = 0;
    int i = 0;
    while (arg[i])
    {
        size += strlen(arg[i]);
        char *tmp = realloc(s, (size + 1) * sizeof(char));
        if (!tmp)
        {
            free(s);
            return NULL;
        }
        s = tmp;
        strcat(s, arg[i]);
        i++;
    }
    return s;
}

char **split_arg(char **arg, enum quotes *enclosure)
{
    int size = array_len(arg) + 1;
    char **new = calloc(size, sizeof(char *));
    if (!new)
        return NULL;
    int ret_val = 1;
    int i = 0;
    int i_new = 0;
    while (arg[i] != NULL && ret_val)
    {
        ret_val = expand_s(new + i_new, arg[i], enclosure[i]);
        char *s = *(new + i_new);
        // printf("%s\n", *(new + i_new));
        int j = 0;
        int start = 0;
        while (s[j] != '\0')
        {
            start = 0;
            if (str_in(shell->ifs, s[j]))
            {
                start = 1;
                size++;
                char **tmp = realloc(new, size * sizeof(char *));
                if (!tmp)
                    return NULL;
                new = tmp;
                new[i_new + 1] =
                    calloc(strlen(new[i_new] + j + 1) + 1, sizeof(char));
                strcpy(new[i_new + 1], new[i_new] + j + 1);
                new[i_new][j] = '\0';
                s = new[++i_new];
                j = 0;
            }
            else
                j++;
        }
        if (start == 0)
            i_new++;
        i++;
    }
    new[i_new] = NULL;
    if (!ret_val)
        return NULL;
    return new;
}

int atoi_begining(char *s)
{
    int i = 0;
    int nb = 0;
    while (s[i] != '\0')
    {
        if (s[i] < '0' || s[i] > '9')
            break;
        nb = nb * 10 + (s[i++] - '0');
    }
    if (nb == 0 && i == 0)
        return -1;
    return nb;
}