#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
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
    FILE *f = fopen(path, "r");
    if (!f)
    {
        shell->return_code = 127;
        fprintf(stderr, "42sh: %s: not found\n", path);
        return NULL;
    }
    struct stat sb;
    if (!(stat(path, &sb) == 0 && sb.st_mode & S_IXUSR))
    {
        shell->return_code = 126;
        fprintf(stderr, "42sh: %s: Permission denied\n", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);
    buffer = calloc(length + 1, sizeof(char));
    if (buffer)
    {
        fread(buffer, 1, length, f);
    }
    fclose(f);
    return buffer;
}

int dot(char **argv)
{
    struct var *save = shell->var_list;
    shell->var_list = NULL;
    if (!argv[1])
        return 0;
    char *buf = get_file_content(argv[1]);
    if (!buf)
        return shell->return_code;
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
    return shell->return_code;
}
