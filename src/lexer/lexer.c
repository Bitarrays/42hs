#include "lexer.h"

/*static */char **split_in_words(char *input)
{
    // strdup or use input and &input in strtok_r
    char *new = strdup(input);
    char **words = NULL;
    int words_nb = 0;
    char spaces[3] = " \t";
    char *word = NULL;

    while ((word = strtok_r(new, spaces, &new)))
    {
        words = realloc(words, sizeof(char *) * (words_nb + 2));
        words[words_nb] = calloc(strlen(word) + 2, sizeof(char));
        strcpy(words[words_nb], word);
        words_nb++;
    }
    words[words_nb] = NULL;
    free(new);
    return words;
}
