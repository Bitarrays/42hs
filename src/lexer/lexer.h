#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum token_type
{
    TOKEN_ERROR,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_ELIF,
    TOKEN_FI,
    TOKEN_THEN,
    TOKEN_SEPARATOR,
    TOKEN_WORD,
    TOKEN_EOF
};

struct lexer_token
{
    enum token_type type;
    char *value;
    struct lexer_token *next;
};

/**
 *  @var lexer::input
 *  Member 'input' contains the input string.
 *  @var lexer::tokens
 *  Member 'tokens' contains the head of the list of tokens.
 *  @var lexer::tail
 *  Member 'tail' contains the last token of the list.
 */
struct lexer
{
    char *input;
    struct lexer_token *tokens;
    struct lexer_token *tail;
};

/**
** \brief Allocate and init a new lexer.
** \param input the string to use as input stream.
*/
struct lexer *lexer_create(char *input);

/**
** \brief Fill the token list by creating all the tokens from
** the given string.
**
** \param lexer an empty lexer.
*/
void lexer_build(struct lexer *lexer);

/**
** \brief Return the next token without consume it.
**
** \return the next token from the input stream
** \param lexer the lexer to lex from
*/
struct lexer_token *lexer_peek(struct lexer *lexer);

/**
** \brief Return and consume the next token from the input stream.
**
** \return the next token from the input stream
** \param lexer the lexer to lex from
*/
struct lexer_token *lexer_pop(struct lexer *lexer);

/**
** \brief Append a new token to the token_list of the lexer.
**
** \param lexer the lexer.
** \param token the token to append.
*/
void lexer_append(struct lexer *lexer, struct lexer_token *token);

/**
** \brief Free a lexer, all the tokens and tokens values.
**
** \param lexer the lexer.
*/
void lexer_free(struct lexer *lexer);

#endif // !LEXER_H
