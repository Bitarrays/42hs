#include "42sh.h"

#include <stdio.h>
#include <string.h>

#include "lexer/lexer.h"
#include "shell_input/shell_input.h"

struct shell *shell;

static void init_shell(int argc, char **argv)
{
    shell = calloc(1, sizeof(struct shell));
    shell->pretty_print = argc > 1 ? !strcmp(argv[1], "--pretty-print") : false;
    if (shell->pretty_print)
    {
        printf("Pretty print enabled\n");
        argc--;
        argv++;
    }
    shell->verbose = argc > 1 ? !strcmp(argv[1], "--verbose") : false;
    if (shell->verbose)
        printf("Verbose mode enabled\n");
    shell->oldpwd = calloc(2048, sizeof(char));
    if (getcwd(shell->oldpwd, 2048) == NULL)
        shell->exit = true;

    // TODO: what are the shell parameters?
    shell->args = NULL;
    shell->nb_args = 0;

    shell->ifs = calloc(100, sizeof(char));
    strcpy(shell->ifs, " \t\n");
    shell->uid = getuid();
}

void free_shell(void)
{
    free(shell->oldpwd);
    free(shell->ifs);
    free(shell);
}

void print_shell(void)
{
    printf("42sh\n");
    printf("  + oldpwd: %s\n", shell->oldpwd);
    printf("  + args (%d):\n", shell->nb_args);
    for (int i = 0; i < shell->nb_args; i++)
        printf("    %s\n", shell->args[i]);
    printf("  + ifs:");
    for (int i = 0; shell->ifs[i] != '\0'; i++)
        printf(" %d", shell->ifs[i]);
    printf("\n");
    printf("  + uid: %d\n", shell->uid);
    printf("  + exit: %d\n", shell->exit);
    printf("  + last_return_code: %d\n", shell->return_code);
    printf("\n");
}

int main(int argc, char **argv)
{
    init_shell(argc, argv);
    if (shell->exit)
    {
        free_shell();
        fprintf(stderr, "42sh: error during initialization.\n");
        return 1;
    }
    // print_shell();
    int res;
    if (shell->pretty_print && shell->verbose)
        res = get_input(argc - 2, argv + 2);
    else if (shell->pretty_print || shell->verbose)
        res = get_input(argc - 1, argv + 1);
    else
        res = get_input(argc, argv);
    free_shell();
    return res;
    char *input = calloc(49, sizeof(char));
    strcpy(input, "if test then 42sh.c\n then echo '42sh.c exists'\n fi");
    struct lexer *lexer = lexer_create(input);
    lexer_build(lexer);
    lexer_free(lexer);
    return (0);
}
