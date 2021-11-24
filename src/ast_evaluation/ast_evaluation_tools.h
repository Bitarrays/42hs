#ifndef AST_EVALUATION_TOOLS_H
#define AST_EVALUATION_TOOLS_H

// include builtin

// int call_builtin(char *cmd);
int is_builtin(char *);
int call_exec(char **cmd);

#endif /* !AST_EVALUATION_TOOLS_H */
