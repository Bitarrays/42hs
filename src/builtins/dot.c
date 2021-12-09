#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include "42sh.h"
#include "lexer.h"
#include "parser.h"
#include "shell_input.h"
#include "var_list.h"

extern struct shell *shell;

char *get_file_content(const char *path)
{
    char *buffer = 0;
    long length;
    FILE *f = fopen (path, "rb");

    if (f)
    {
        fseek (f, 0, SEEK_END);
        length = ftell (f);
        fseek (f, 0, SEEK_SET);
        buffer = calloc(length + 1, sizeof(char));
        if (buffer)
        {
            fread (buffer, 1, length, f);
        }
        fclose(f);
    }
    return buffer;
}

int dot(char **argv)
{
    struct var *save = shell->var_list;
    shell->var_list = NULL;
    char *buf = get_file_content(argv[0]);
    int res = parse_input(buf);
    free(buf);
    struct var *tmp = shell->var_list;
    while (tmp)
    {
        struct var *next = tmp->next;
        free(tmp->name);
        free(tmp->value);
        free(tmp);
        tmp = next;
    }
    shell->var_list = save;
    return res;
    return 0;
}
