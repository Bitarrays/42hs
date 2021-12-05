#include "ast_evaluation.h"

#include <stdio.h>
#include <string.h>

#include "../builtins/builtins.h"

int evaluate_ast(struct ast *ast)
{
    if (!ast)
        return 0;

    if (ast->type == AST_IF)
    {
        if (!evaluate_ast(ast->condition))
            return evaluate_ast(ast->left_child);
        else
            return evaluate_ast(ast->right_child);
    }
    else if (ast->type == AST_FOR)
    {
        char *oo = "oui non";
        push_elt_list(shell, "name", oo);
        //printf("%s\n", find_elt_list(shell, "name"));
        char *aa = "oui oui";
        push_elt_list(shell, "name", aa);
        //printf("%s\n", find_elt_list(shell, "name"));
        //free_list(shell);
        //char **val = expand(ast);
        //val = split_arg(val, ast->enclosure);
        char **var;
        char *test[3] = {"oui", "non", NULL};
        enum quotes enclosure2[2] = {Q_DOUBLE, Q_DOUBLE};
        //printf("%s\n", ast->value[0]);
        if (!ast->value[1] || !ast->value[2])
        {
            var = split_arg(test, enclosure2);
            // wait impletation of env var
            //enum quotes enclosure[1] = {Q_NONE};
            //var = split_arg(shell->args, enclosure); // add var in array
        }
        else
            var = split_arg(expand(ast), ast->enclosure);

        if (var && ast->value)
        {
            int i = 0;
            while (var[i])
            {
                push_elt_list(shell, ast->value[0], var[i++]);
                evaluate_ast(ast->left_child);
            }
        }
        return evaluate_ast(ast->right_child); // check return code for a null for
    }
    else if (ast->type == AST_WHILE)
    {
        int ret = 0;
        while (!evaluate_ast(ast->condition))
            ret = evaluate_ast(ast->left_child);
        return ret;
    }
    else if (ast->type == AST_UNTIL)
    {
        int ret = 0;
        while (evaluate_ast(ast->condition))
            ret = evaluate_ast(ast->left_child);
        return ret;
    }
    else if (ast->type == AST_AND || ast->type == AST_OR)
    {
        int prec = !evaluate_ast(ast->left_child);
        while (ast->right_child
               && (ast->right_child->type == AST_OR
                   || ast->right_child->type == AST_AND))
        {
            if (ast->type == AST_AND)
                prec = prec && !evaluate_ast(ast->right_child->left_child);
            else if (ast->type == AST_OR)
                prec = prec || !evaluate_ast(ast->right_child->left_child);
            ast = ast->right_child;
        }
        if (ast->type == AST_AND)
            prec = prec && !evaluate_ast(ast->right_child);
        else if (ast->type == AST_OR)
            prec = prec || !evaluate_ast(ast->right_child);
        return !prec;
        /*return !evaluate_ast(ast->left_child)
            && !evaluate_ast(ast->right_child);*/
    }
    /*else if (ast->type == AST_OR)
    {
        return !evaluate_ast(ast->left_child)
            || !evaluate_ast(ast->right_child);
    }*/
    else if (ast->type == AST_REDIR)
    {
        int nb = 1;
        struct ast *tmp = ast;
        while (tmp->right_child->type == AST_REDIR)
        {
            tmp = tmp->right_child;
            nb++;
        }
        char ***redirs = calloc(nb + 1, sizeof(char **));
        if (!redirs)
            return 1;
        while (ast->right_child->type)
        {
            continue;
        }
        //int fd = atoi_begining(char *s);
        printf("%d\n", nb);
        printf("%s\n", ast->left_child->value[0]);
    }
    else if (ast->type == AST_PIPE)
    {
        int nb = 1;
        struct ast *tmp = ast;
        while (tmp->right_child->type == AST_PIPE)
        {
            tmp = tmp->right_child;
            nb++;
        }
        char ***redirs = calloc(nb + 2, sizeof(char **));
        if (!redirs)
            return 1;
        int i = 0;
        while (ast->right_child->type)
        {
            redirs[i] = ast->left_child->value;
            ast = ast->right_child;
        }
        printf("nb %d\n", nb);
        redirs[i] = ast->right_child->value;
        exec_pipe(redirs, nb + 1);
    }
    else if (ast->type == AST_ASSIGNMENT)
    {
        char **var = expand(ast);
        char *val = merge_arg(var);
        push_elt_list(shell, ast->var_name, val);
    }
    else if (ast->type == AST_COMMAND)
    {
        char *oo = "oui non";
        push_elt_list(shell, "name", oo);
        // printf("%s\n", find_elt_list(shell, "name"));
        char *aa = "oui oui";
        push_elt_list(shell, "name", aa);
        // printf("%s\n", find_elt_list(shell, "name"));
        // free_list(shell);
        char **val = expand(ast);
                //printf("%s\n", val[0]);
        // val = split_arg(val, ast->enclosure);
        if (!val)
            return 1;
        int res;
        if (is_builtin(*(val)))
            res = find_command(val);
        else
            res = call_exec(val);
        char *tmp = val[0];
        int pos = 0;
        while (tmp)
        {
            free(tmp);
            tmp = val[++pos];
        }
        free(val);
        return res;
    }
    else if (ast->type == AST_NOT)
        return !evaluate_ast(ast->left_child);
    else if (ast->type == AST_LIST)
    {
        int r = evaluate_ast(ast->right_child);
        if (ast->left_child && ast->left_child->type != AST_EOF)
            return evaluate_ast(ast->left_child);
        return r;
    }
    else
    {
        printf("Not implemented yet\n");
    }

    return 0;
}
