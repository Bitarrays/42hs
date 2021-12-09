#include "ast_evaluation.h"

#include <stdio.h>
#include <string.h>

#include "builtins.h"
#include "redir.h"

int evaluate_ast(struct ast *ast)
{
    if (shell->exit/* || shell->continue || shell->break*/)
        return shell->return_code;
    if (!ast)
        return 0;

    if (ast->type == AST_IF)
    {
        if (!evaluate_ast(ast->condition))
        {
            if (shell->exit/* || shell->continue || shell->break*/)
                return shell->return_code;
            return evaluate_ast(ast->left_child);
        }
        else
        {
            if (shell->exit/* || shell->continue || shell->break*/)
                return shell->return_code;
            return evaluate_ast(ast->right_child);
        }
    }
    /*if (ast->type == AST_FUNC)
    {
        new_var(shell, ast->value);
        int res = ast_evaluation(ast->left_child);
        del_stack(shell);
        push_int_elt_list(shell, "?", res);
        res = ast_evaluation(ast->right_child);
        return res;
    }*/
    else if (ast->type == AST_FOR)
    {
        char **var;
        char *test[3] = { "oui", "non", NULL };
        enum quotes enclosure2[2] = { Q_DOUBLE, Q_DOUBLE };
        if (!ast->value[1] || !ast->value[2])
        {
            var = split_arg(test, enclosure2);
            // wait impletation of env var
            // enum quotes enclosure[1] = {Q_NONE};
            // var = split_arg(shell->args, enclosure); // add var in array
        }
        else
            var = split_arg(
                ast->value + 2 /*expand(ast->value + 2, ast->enclosure + 2)*/,
                ast->enclosure + 2);
        if (var && ast->value)
        {
            int i = 0;
            while (var[i])
            {
                push_elt_list(shell, ast->value[0], var[i++]);
                evaluate_ast(ast->left_child);
                if (shell->exit/* || shell->continue || shell->break*/)
                    return shell->return_code;
            }
        }
        free_arg(var);
        int res = evaluate_ast(
            ast->right_child); // check return code for a null for
        if (shell->exit/* || shell->continue || shell->break*/)
            return shell->return_code;
        return res;
    }
    else if (ast->type == AST_WHILE)
    {
        int ret = 0;
        while (!evaluate_ast(ast->condition))
        {
            if (shell->exit/* || shell->continue || shell->break*/)
                return shell->return_code;
            ret = evaluate_ast(ast->left_child);
        }
        if (shell->exit/* || shell->continue || shell->break*/)
            return shell->return_code;
        return ret;
    }
    else if (ast->type == AST_UNTIL)
    {
        int ret = 0;
        while (evaluate_ast(ast->condition))
        {
            if (shell->exit/* || shell->continue || shell->break*/)
                return shell->return_code;
            ret = evaluate_ast(ast->left_child);
        }
        if (shell->exit/* || shell->continue || shell->break*/)
            return shell->return_code;
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
            /*if (shell->continue)
            {
                
            }
            else if (shell->break)
            {

            }*/
            if (shell->exit)
                return shell->return_code;
            ast = ast->right_child;
        }
        if (ast->type == AST_AND)
            prec = prec && !evaluate_ast(ast->right_child);
        else if (ast->type == AST_OR)
            prec = prec || !evaluate_ast(ast->right_child);
        if (shell->exit/* || shell->continue || shell->break*/)
            return shell->return_code;
        return !prec;
    }
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
        int i = 0;
        tmp = ast;
        while (tmp->right_child->type)
        {
            redirs[i] = calloc(4, sizeof(char *));
            if (!redirs[i])
                return 1; // error a recheck
            int j = 0;
            while (tmp->value[j])
            {
                redirs[i][j] = tmp->value[j];
                j++;
            }
            tmp = tmp->right_child;
            redirs[i++][j] = tmp->left_child->value[0];
        }
        redirs[i] = calloc(4, sizeof(char *));
        if (!redirs[i])
            return 1; // error a recheck
        int j = 0;
        while (tmp->value[j])
        {
            redirs[i][j] = tmp->value[j];
            j++;
        }
        redirs[i][j] = tmp->right_child->value[0];

        int k = 0;
        while (redirs[k] && shell->verbose)
        {
            int l = 0;
            while (redirs[k][l])
            {
                printf("%s ", redirs[k][l++]);
            }
            k++;
            printf("\n");
        }
        // call redir
        exec_redirections(redirs);
        int res = evaluate_ast(ast->left_child);
        if (shell->exit/* || shell->continue || shell->break*/)
            return shell->return_code;
        return res;
        // int fd = atoi_begining(char *s);
        /*if (shell->verbose)
        {
            printf("%d\n", nb);
            printf("%s\n", ast->left_child->value[0]);
        }*/
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
        enum quotes **enclosure = calloc(nb + 2, sizeof(enum quotes *));
        if (!redirs)
            return 1;
        redirs[0] = ast->left_child->value;
        enclosure[0] = ast->left_child->enclosure;
        int i = 1;
        while (ast->right_child->type == AST_PIPE)
        {
            enclosure[i] = ast->left_child->enclosure;
            redirs[i++] = ast->left_child->value;
            ast = ast->right_child;
        }
        redirs[i] = ast->right_child->value;
        enclosure[i] = ast->right_child->enclosure;
        int res = exec_pipe(redirs, enclosure, nb);
        push_int_elt_list(shell, "?", res);
        return res;
    }
    else if (ast->type == AST_ASSIGNMENT)
    {
        if (ast->value)
        {
            char **var = expand(ast->value, ast->enclosure);
            char *val = merge_arg(var);
            push_elt_list(shell, ast->var_name, val);
            free_arg(var);
            free(val);
        }
    }
    else if (ast->type == AST_COMMAND)
    {
        char **val = expand(ast->value, ast->enclosure);
        // val = split_arg(val, ast->enclosure);
        if (!val)
            return 1;
        int res;
        if (is_builtin(*(val)))
            res = find_command(val, 1);
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
        push_int_elt_list(shell, "?", res);
        return res;
    }
    else if (ast->type == AST_NOT)
        return !evaluate_ast(ast->left_child);
    else if (ast->type == AST_LIST)
    {
        int r = evaluate_ast(ast->right_child);
        // printf("%d\n", shell->exit);
        if (shell->exit/* || shell->continue || shell->break*/)
        {
            // printf("%d\n", shell->return_code);
            return shell->return_code;
        }
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
