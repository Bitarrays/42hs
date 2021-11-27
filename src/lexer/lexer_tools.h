#ifndef LEXER_TOOLS_H
#define LEXER_TOOLS_H

#include "lexer.h"

void words_to_ionumber(struct lexer *lexer);

void create_and_append_token(struct lexer *lexer, enum token_type type,
                             char *value);

char **split_in_words(char *input);

#endif // !LEXER_TOOLS_H
