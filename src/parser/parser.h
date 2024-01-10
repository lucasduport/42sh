#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

enum parser_status
{
    PARSER_OK,
    PARSER_UNEXPECTED_TOKEN,
};

/**
 * @brief Parse global grammar :
 * 
 *   list '\n'
 * | list EOF
 * | '\n'
 * | EOF
 * 
 * @param parameter1 lexer used in current execution
 * @param parameter2 result ast of parsing
*/
enum parser_status parser_input(struct lexer *lex, struct ast **res);


/**
 * @brief Parse list grammar :
 * 
 * and_or {';' and_or} [';']
*/
enum parser_status parser_list(struct lexer *lex, struct ast **res);

/**
 * @brief Parse and and or grammar :
 * 
 * pipeline
*/
enum parser_status parser_and_or(struct lexer *lex, struct ast **res);

/**
 * @brief Parse pipeline grammar :
 * 
 * command
*/
enum parser_status parser_pipeline(struct lexer *lex, struct ast **res);

/**
 * @brief Parse command grammar :
 * 
 *   simple_command
 * | shell command
*/
enum parser_status parser_command(struct lexer *lex, struct ast **res);

/**
 * @brief Parse simple command grammar :
 * 
 * WORD {element}
*/
enum parser_status parser_simple_command(struct lexer *lex, struct ast **res);

/**
 * @brief Parse element grammar :
 * 
 * WORD
*/
enum parser_status parser_element(struct lexer *lex, struct ast **res);

/**
 * @brief Parse shell command grammar :
 * 
 * rule_if
*/
enum parser_status parser_shell_command(struct lexer *lex, struct ast **res);

/**
 * @brief Parse if grammar :
 * 
 * 'if' compund_list 'then' compound_list [else clause] 'fi'
*/
enum parser_status parser_rule_if(struct lexer *lex, struct ast **res);

/**
 * @brief Parse else clause grammar :
 * 
 *   'else' compund_list
 * | 'elif' compound_list 'then' compound_list [else clause]
*/
enum parser_status parser_else_clause(struct lexer *lex, struct ast **res);

#endif PARSER_H /* !PARSER_H */