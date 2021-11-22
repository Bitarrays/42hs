#ifndef AST_H
#define AST_H

enum AST_NODE
{
    AST_COMMAND = 0,
    AST_LIST, // < ('\n')* and_or ((';'|'\n') ('\n')* and_or)* [(';'|'\n') ('\n')*]
    AST_IF,
    AST_FOR,
    AST_WHILE,
    AST_CASE,
    AST_PIPE,
    AST_OR,
    AST_AND,
    AST_REDIR,
    AST_FUNC
};

struct ast
{
    enum AST_NODE type;
    char *value;
    struct ast *left_child;
    struct ast *right_child;
    struct ast *condition;
};

#endif /* !AST_H */
