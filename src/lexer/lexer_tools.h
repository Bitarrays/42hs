#ifndef LEXER_TOOLS_H
#define LEXER_TOOLS_H

#include "lexer.h"
#include "spaces.h"

void create_and_append_token(struct lexer *lexer, enum token_type type,
                             char *value);

char **split_in_words(char *input);

enum token_type get_keyword(char *word);

bool is_keyword(char *word);

bool is_int(char *word);

#endif // !LEXER_TOOLS_H
