#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

enum parser_status
{
    PARSER_OK = 0,
    PARSER_ERROR
};

/**
 ** @brief Check if input grammar rule is respected
 ** >> input: compound_list EOF | EOF;
 ** 
 ** @param ast the general ast to update 
 ** @param lexer the lexer to read tokens from
 ** @return enum parser_status - current parser status
 **/
enum parser_status parse_input(char *input);



/**
 ** @brief Check if compound_list grammar rule is respected
 ** >> compound_list: and_or (';' and_or)* [';'];
 ** 
 ** @param ast the general ast to update 
 ** @param lexer the lexer to read tokens from
 ** @return enum parser_status - current parser status
 **/
enum parser_status parse_compound_list(struct ast **ast, struct lexer *lexer);



/**
 ** @brief Check if simple_command grammar rule is respected
 ** >> simple_command: WORD*
 ** 
 ** @param ast the general ast to update 
 ** @param lexer the lexer to read tokens from
 ** @return enum parser_status - current parser status
 **/
enum parser_status parse_simple_command(struct ast **ast, struct lexer *lexer);



/**
 ** @brief Check if shell_command grammar rule is respected
 ** >> shell_command: rule_if
 ** 
 ** @param ast the general ast to update 
 ** @param lexer the lexer to read tokens from
 ** @return enum parser_status - current parser status
 **/
enum parser_status parse_shell_command(struct ast **ast, struct lexer *lexer);



/**
 ** @brief Check if command grammar rule is respected
 ** >> command: simple_command | shell_command
 ** 
 ** @param ast the general ast to update 
 ** @param lexer the lexer to read tokens from
 ** @return enum parser_status - current parser status
 **/
enum parser_status parse_command(struct ast **ast, struct lexer *lexer);



/**
 ** @brief Check if pipeline grammar rule is respected
 ** >> pipeline: command
 ** 
 ** @param ast the general ast to update 
 ** @param lexer the lexer to read tokens from
 ** @return enum parser_status - current parser status
 **/
enum parser_status parse_pipeline(struct ast **ast, struct lexer *lexer);



/**
 ** @brief Check if and_or grammar rule is respected
 ** >> and_or: pipeline
 ** 
 ** @param ast the general ast to update 
 ** @param lexer the lexer to read tokens from
 ** @return enum parser_status - current parser status
 **/
enum parser_status parse_and_or(struct ast **ast, struct lexer *lexer);



/**
 ** @brief Check if rule_if grammar rule is respected
 ** >> rule_if: If compound_list Then compound_list [else_clause] Fi
 ** 
 ** @param ast the general ast to update 
 ** @param lexer the lexer to read tokens from
 ** @return enum parser_status - current parser status
 **/
enum parser_status parse_rule_if(struct ast **ast, struct lexer *lexer);



/**
 ** @brief Check if else_clause grammar rule is respected
 ** >> else_clause: Else compound_list | Elif compound_list Then compound_list [else_clause]
 ** 
 ** @param ast the general ast to update 
 ** @param lexer the lexer to read tokens from
 ** @return enum parser_status - current parser status
 **/
enum parser_status parse_else_clause(struct ast **ast, struct lexer *lexer);

#endif // !PARSER_H
