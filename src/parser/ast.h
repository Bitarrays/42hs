#ifndef AST_H
#define AST_H

enum ast_type
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
    enum ast_type type;
    char *value;
    struct ast *left_child;
    struct ast *right_child;
    struct ast *condition;
};

<<<<<<< HEAD
/**
 ** \brief Allocate a new ast with the given type
 */
struct ast *ast_new(enum ast_type type);

/**
 ** \brief Recursively free the given ast
 */
void ast_free(struct ast *ast);

#endif /* !AST_H */
=======
#endif /* !AST_H */
>>>>>>> cd1d22fd7cf3f1eaca085d9d07675fa2a3576022
