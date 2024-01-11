#ifndef PARSER_H
#define PARSER_H

#include "../utils/ast/ast.h"
#include "../lexer/lexer.h"
#include "../lexer/token.h"

enum parser_status
{
    PARSER_OK,
    PARSER_UNEXPECTED_TOKEN,
    PARSER_ARG_ERROR
};

/**
 * @brief Initialize lexer and parse imput
 * 
 * @param parameter1 argc given in main
 * @param parameter2 argv given in main
 * @return parser_status > PARSER_ARG_ERROR if lexing failed
 *                      > PARSER_UNEXPECTED_TOKEN if parsing failed
 *                      > PARSER_OK otherwhise
*/
enum parser_status parser(int argc, char *argv[], struct ast **res);

#endif /* !PARSER_H */
