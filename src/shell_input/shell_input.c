#include "shell_input.h"

#include "../parser/parser.h"

static int shell_prompt(void)
{
    int c = '\n';
    char *input = NULL;
    int input_len = 0;
    while (true)
    {
        printf("\033[1m\033[32m➜  \033[1m\033[36m42sh$ \033[0;37m");
        fflush(stdout);
        while (read(STDIN_FILENO, &c, 1) > 0)
        {
            if (c == EOF)
                return 0;
            if (c == '\n')
                break;
            input = realloc(input, (input_len + 2) * sizeof(char));
            input[input_len++] = c;
        }
        if (!input)
            continue;
        input[input_len] = 0;
        parse_input(input);
        free(input);
        input = NULL;
        input_len = 0;
    }
    return 0;
}

static char *get_file_content(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "42sh: Can't open %s\n", filename);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *content = calloc(size + 1, sizeof(char));
    fread(content, 1, size, file);
    content[size] = 0;
    fclose(file);
    return content;
}

int get_input(int argc, char **argv)
{
    char *input = NULL;
    size_t input_len = 0;
    if (argc < 2)
    {
        int c;
        if (!isatty(STDIN_FILENO))
        {
            while (read(STDIN_FILENO, &c, 1) > 0)
            {
                if (c == EOF)
                    break;
                input = realloc(input, (input_len + 2) * sizeof(char));
                input[input_len++] = c;
            }
        }
        else
            return shell_prompt();
    }
    else
    {
        int i = 1;
        if (!strcmp(argv[i], "-c"))
            input = strdup(argv[++i]);
        else
            input = get_file_content(argv[i]);
        if (!input)
            return 1;
        input_len = strlen(input);
    }
    if (input)
    {
        input[input_len] = '\0';
        parse_input(input);
        free(input);
    }
    return 0;
}
