#ifndef AST_EVALUATION_TOOLS_H
#define AST_EVALUATION_TOOLS_H

#include "../42sh.h"
#include "../parser/ast.h"
#include "../variable/var_list.h"

// include builtin
extern struct shell *shell;
// int call_builtin(char *cmd);
int is_builtin(char *);
int call_exec(char **cmd);
int is_in(char **condition);
char **expand(struct ast *ast);
char **split_arg(char **arg, enum quotes *enclosure);
char *merge_arg(char **arg);
int atoi_begining(char *s);
void exec_pipe(char ***args, int pipe_nb);

#endif /* !AST_EVALUATION_TOOLS_H */
