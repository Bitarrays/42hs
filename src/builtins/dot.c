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

// static char **split_path(char *path)
// {
//     char *rest = path;
//     char *token;
//     char **paths = NULL;
//     int paths_nb = 0;
//     while ((token = strtok_r(rest, ":", &rest)) != NULL)
//     {
//         paths = realloc(paths, sizeof(char *) * (paths_nb + 2));
//         paths[paths_nb++] = token;
//     }
//     paths[paths_nb] = NULL;
//     return paths;
// }

// static char *find_in_path(const char *input)
// {
//     char *path=getenv("PATH");
//     if (!path)
//     {
//         fprintf(stderr, "42sh: .: %s: not found\n", input);
//         shell->return_code = 2;
//         return NULL;
//     }
//     char **path_single = split_path(path);
//     if (!path_single)
//     {
//         fprintf(stderr, "42sh: .: %s: not found\n", input);
//         shell->return_code = 2;
//         return NULL;
//     }
//     int i = 0;
//     bool found = false;
//     char *final_path = NULL;
//     while (path_single[i] && !found)
//     {
//         char *full_path = calloc(strlen(path_single[i]) + strlen(input) + 2, sizeof(char));
//         strcpy(full_path, path_single[i]);
//         strcat(full_path, "/");
//         strcat(full_path, input);
//         if (access(full_path, F_OK) == 0)
//         {
//             found = true;
//             final_path = full_path;
//             continue;
//         }
//         free(full_path);
//         i++;
//     }
//     free(path_single);
//     struct stat sb;
//     if (!final_path)
//     {
//         fprintf(stderr, "42sh: .: %s: not found\n", input);
//         shell->return_code = 127;
//     }
//     else if (!(stat(final_path, &sb) == 0 && sb.st_mode & S_IXUSR))
//     {
//         fprintf(stderr, "42sh: .: %s: Permission denied\n", input);
//         shell->return_code = 126;
//         free(final_path);
//         final_path = NULL;
//     }
//     return final_path;
// }

static char get_first_char(const char *arg)
{
    int pos = 0;
    while (arg[pos] != 0 && arg[pos] == '.')
        pos++;
    return arg[pos];
}

char *get_file_content(const char *path)
{
    if (get_first_char(path) != '/')
        return NULL;
    if (!path)
        return NULL;
    char *buffer = 0;
    long length;
    FILE *f = fopen(path, "r");
    if (!f)
    {
        shell->return_code = 127;
        fprintf(stderr, "42sh: .: Can't open %s\n", path);
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
