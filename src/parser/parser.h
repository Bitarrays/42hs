#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

enum parser_status
{
    PARSER_OK = 0,
    PARSER_ERROR
};

int parser(void);

#endif // !PARSER_H
