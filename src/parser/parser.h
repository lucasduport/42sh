#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"
#include "../lexer/token.h"
#include "../utils/ast/ast.h"

enum parser_status
{
    PARSER_OK,
    PARSER_ERROR,
    PARSER_EOF
};

/**
 * @file parser.c
 * @brief Parse global grammar :
 *
 *   list '\n'
 * | list EOF
 * | '\n'
 * | EOF
 *
 * @param lex Lexer used in current execution
 * @param res Result ast of parsing
 * @return enum parser_status OK or UNEXPECTED_TOKEN if there is an error
 */
enum parser_status parser_input(struct lexer *lex, struct ast **res);

/**
 * @file parser.c
 * @brief Parse and and or grammar :
 *
 * pipeline { ('&&' | '||'){'\n'} pipeline }
 */
enum parser_status parser_and_or(struct lexer *lex, struct ast **res);

/**
 * @file parser.c
 * @brief Parse element grammar :
 *
 * WORD
 */
enum parser_status parser_element(struct lexer *lex, struct ast **res);

/**
 * @file parser.c
 * @brief Skip optional newline
 */
void skip_newline(struct lexer *lex);

/**
 * @file parser_command.c
 * @brief Parse command grammar :
 *
 *   simple_command
 * | shell command { redirection }
 * | fundec { redirection }
 */
enum parser_status parser_command(struct lexer *lex, struct ast **res);

/**
 * @file parser_command.c
 * @brief Parse simple command grammar :
 *
 *   prefix { prefix }
 * | { prefix } WORD { element }
 */
enum parser_status parser_simple_command(struct lexer *lex, struct ast **res,
                                         struct token *w);

/**
 * @file parser_command.c
 * @brief Parse shell command grammar :
 *
 *   rule_if
 * | rule_while
 * | rule_until
 * | rule_for
 * | '{' compund_list '}'
 * | '(' compund_list ')'
 */
enum parser_status parser_shell_command(struct lexer *lex, struct ast **res);

/**
 * @file parser_command.c
 * @brief Parse fundec grammar :
 *
 * WORD '(' ')' {'\n'} shell_command
 */
enum parser_status parser_fundec(struct lexer *lex, struct ast **res,
                                 struct token *w);

/**
 * @file parser_if.c
 * @brief Parse if grammar :
 *
 * 'if' compund_list 'then' compound_list [else clause] 'fi'
 */
enum parser_status parser_rule_if(struct lexer *lex, struct ast **res);

/**
 * @file parser_if.c
 * @brief Parse else clause grammar :
 *
 *   'else' compund_list
 * | 'elif' compound_list 'then' compound_list [else clause]
 */
enum parser_status parser_else_clause(struct lexer *lex, struct ast **res);

/**
 * @file parser_if.c
 * @brief Parse case grammar :
 *
 * 'case' WORD {'\n'} 'in' {'\n'} [case_clause] 'esac'
 */
enum parser_status parser_rule_case(struct lexer *lex, struct ast **res);

/**
 * @file parser_if.c
 * @brief Parse case clause grammar :
 *
 * case_item {';;' {'\n'} case_item } [';;'] {'\n'}
 */
enum parser_status parser_case_clause(struct lexer *lex, struct ast **res);

/**
 * @file parser_if.c
 * @brief Parse case item grammar :
 *
 * ['('] WORD {'|' WORD } ')' {'\n'} [compund_list]
 */
enum parser_status parser_case_item(struct lexer *lex, struct ast **res);

/**
 * @file parser_list.c
 * @brief Parse list grammar :
 *
 * and_or {';' and_or} [';']
 */
enum parser_status parser_list(struct lexer *lex, struct ast **res);

/**
 * @file parser_list.c
 * @brief Parse compound_list grammar :
 *
 *  {'\n'} and_or { (';' | '\n') {'\n'} and_or} [';'] {'\n'}
 */
enum parser_status parser_compound_list(struct lexer *lex, struct ast **res);

/**
 * @file parser_redir.c
 * @brief Parse pipeline grammar :
 *
 * ['!'] command { '|' {'\n'} command }
 */
enum parser_status parser_pipeline(struct lexer *lex, struct ast **res);

/**
 * @file parser_redir.c
 * @brief Parse prefix grammar :
 *
 *   ASSIGNMENT_WORD
 * | redirection
 */
enum parser_status parser_prefix(struct lexer *lex, struct ast **res);

/**
 * @file parser_redir.c
 * @brief Parse prefix grammar :
 *
 * [ IO_NUMBER ] TOKEN_REDIR WORD
 */
enum parser_status parser_redirection(struct lexer *lex, struct ast **res);

/**
 * @file parser_loop.c
 * @brief Parse while grammar :
 *
 * 'while' compund_list 'do' compund_list 'done'
 */
enum parser_status parser_rule_while(struct lexer *lex, struct ast **res);

/**
 * @file parser_loop.c
 * @brief Parse until grammar :
 *
 * 'until' compund_list 'do' compund_list 'done'
 */
enum parser_status parser_rule_until(struct lexer *lex, struct ast **res);

/**
 * @file parser_loop.c
 * @brief Parse for grammar :
 *
 * 'for' WORD ([';'] | [ {'\n'} 'in' {WORD} (';' | '\n') ] ) {'\n'} 'do'
 * compund_list 'done'
 */
enum parser_status parser_rule_for(struct lexer *lex, struct ast **res);

#endif /* !PARSER_H */
