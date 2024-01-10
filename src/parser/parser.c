#include "parser.h"

#include <stdio.h>

enum parser_status parser_input(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);
    *res = NULL;

    //first token = EOF or NEWLINE -> OK
    if (peek.type == TOKEN_EOF || peek_type == TOKEN_NEWLINE)
        return PARSER_OK;

    if (peek.type == TOKEN_ERROR || parse_list(lex, res) == PARSER_UNEXPECTED_TOKEN)
        goto error;

    // parse_list works => there must be an EOF or NEWLINE.
    peek = lexer_peek(lex);
    if (peek.type == TOKEN_EOF || peek_type == TOKEN_NEWLINE)
    {
        //Create ast RES with type AST_LIST
        struct ast *tmp = *res;
        *res = ast_new(AST_LIST);
        (*res)->first_child = tmp;
        return PARSER_OK;
    }

    // If not.
    goto error;

error:
    fprintf(stderr, "parser: parsing error\n");
    ast_free(*res);
    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parser_list(struct lexer *lex, struct ast **res)
{
    if (parser_and_or(lex, res) == PARSER_UNEXPECTED_TOKEN)
        return PARSER_UNEXPECTED_TOKEN;
    
    struct token peek = lexer_peek(lex);

    while (peek.type == TOKEN_SEMICOLONS)
    {
        lexer_pop(lex);
        peek = lexer_peek(lex);

        //';' is the last optional => EOF or '\n' -> follow of list
        if (peek.type == TOKEN_EOF || peek.type == TOKEN_NEWLINE)
            return PARSER_OK;
        
        struct ast *tmp_ast = NULL;
        //Else we need to parse 'and_or'
        if (parser_and_or(lex, &tmp_ast) == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free(tmp_ast);
            return PARSER_UNEXPECTED_TOKEN;
        }
    
        //If and_or works, add command retrieving
        ast_add_brother(*res, tmp_ast);
        peek = lexer_peek(lex);

    }
    return PARSER_OK;
}

enum parser_status parser_and_or(struct lexer *lex, struct ast **res)
{
    return parser_pipeline(lex, res);
}

enum parser_status parser_pipeline(struct lexer *lex, struct ast **res)
{
    return parser_command(lex, res);
}

enum parser_status parser_command(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

    //TOKEN_WORD -> first of simple_command
    if (peek.type == TOKEN_WORD)
        return parser_simple_command(lex, res);
    
    //TOKEN_IF -> first of shell_command
    if (peek.type == TOKEN_IF)
        return parser_shell_command(lex, res);
    
    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parser_simple_command(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lexer);

    if (peek.type != TOKEN_WORD)
        return PARSER_UNEXPECTED_TOKEN;
    lexer_pop(lexer);

    *res = ast_new(AST_COMMAND);
    (*res)->arg = list_create(peek.data);

    peek = lexer_peek(lexer);
    //EOF and '\n' -> follow of parser_simple_command
    while (peek.type != TOKEN_EOF && peek.type != TOKEN_NEWLINE && parser_element(lex, res) == PARSER_OK)
    {
        //pop because peek (and not pop) in element
        peek = lexer_pop(lexer);
        list_append((*res)->arg, peek.data);
    }

    //If while boucle exit properly (with EOF or '\n')
    if (peek.type != TOKEN_EOF && peek.type != TOKEN_NEWLINE)
        return PARSER_OK;
    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parser_element(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lexer);
    
    if (peek.type == TOKEN_WORD)
        return PARSER_OK;
    
    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parser_shell_command(struct lexer *lex, struct ast **res)
{
    return parser_rule_if(lex, res);
}

enum parser_status parser_rule_if(struct lexer *lex, struct ast **res)
{
    struct token *peek = lexer_peek(lex);

    if (peek.type == TOKEN_IF)
    {
        lexer_pop(lex);
        *res = ast_new(AST_IF);

        //PARSE CONDITION
        struct ast *tmp_condition = NULL;

        if (parser_compound_list(lex, &tmp_condition) == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free(tmp_condition);
            return PARSER_UNEXPECTED_TOKEN;
        }

        (*res)->first_child = tmp_condition;

        //PARSE THEN
        peek = lexer_peek(lex);
        if (peek.type != TOKEN_THEN)
            return PARSER_UNEXPECTED_TOKEN;

        lexer_pop(lex);

        struct ast *tmp_then = NULL;

        if (parser_compound_list(lex, &tmp_then) == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free(tmp_then);
            return PARSER_UNEXPECTED_TOKEN;
        }

        //add brother at tmp_condition (and not res) because that is IF node that have different child
        ast_add_brother(tmp_condition, tmp_then);

        //PARSE ELSE (IF THERE IS)
        peek = lexer_peek(lex);

        if (peek.type == TOKEN_ELSE)
        {
            struct ast *tmp_else = NULL;
            if (parser_else_clause(lex, &tmp_else) == PARSER_UNEXPECTED_TOKEN)
            {
                ast_free(tmp_else);
                return PARSER_UNEXPECTED_TOKEN;
            }
            
            ast_add_brother(tmp_condition, tmp_else);
        }

        //PARSE FI
        peek = lexer_peek(lex);
        if (peek.type == TOKEN_FI)
        {
            leer_pop(lex);
            return PARSER_OK;
        }
    }
    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parser_else_clause(struct lexer *lex, struct ast **res)
{
    struct token *peek = lexer_peek(lex);
    if (peek.type == TOKEN_ELSE)
    {
        lexer_pop(lex);
        return parser_compound_list(lex, res);
    }

    else if (peek.type == TOKEN_ELIF)
    {
        lexer_pop(lex);
        *res = ast_new(AST_IF);

        //PARSE CONDITION
        struct ast *tmp_condition = NULL;

        if (parser_compound_list(lex, &tmp_condition) == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free(tmp_condition);
            return PARSER_UNEXPECTED_TOKEN;
        }

        (*res)->first_child = tmp_condition;

        //PARSE THEN
        peek = lexer_peek(lex);
        if (peek.type != TOKEN_THEN)
            return PARSER_UNEXPECTED_TOKEN;

        lexer_pop(lex);

        struct ast *tmp_then = NULL;

        if (parser_compound_list(lex, &tmp_then) == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free(tmp_then);
            return PARSER_UNEXPECTED_TOKEN;
        }

        //add brother at tmp_condition (and not res) because that is IF node that have different child
        ast_add_brother(tmp_condition, tmp_then);

        //PARSE ELSE (IF THERE IS)
        peek = lexer_peek(lex);

        if (peek.type == TOKEN_ELSE)
        {
            struct ast *tmp_else = NULL;
            if (parser_else_clause(lex, &tmp_else) == PARSER_UNEXPECTED_TOKEN)
            {
                ast_free(tmp_else);
                return PARSER_UNEXPECTED_TOKEN;
            }
            
            ast_add_brother(tmp_condition, tmp_else);
        }
    }

    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parser_compound_list(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);
    while (peek.type == TOKEN_NEWLINE)
    {
        lexer_pop(lex);
        peek = lexer_peek(lex);
    }

    if (parser_and_or(lex, res) == PARSER_UNEXPECTED_TOKEN)
        return PARSER_UNEXPECTED_TOKEN;
    
    peek = lexer_peek(lex);

    while (peek.type == TOKEN_SEMICOLONS || peek.type == TOKEN_NEWLINE)
    {
        lexer_pop(lex);
        peek = lexer_peek(lex);

        //Skip '\n'
        while(peek.type == TOKEN_NEWLINE)
        {
            lexer_pop(lex);
            peek = lexer_peek(lex);
        }

        if(peek.type == TOKEN_ELSE || peek.type == TOKEN_ELIF || peek.type == TOKEN_THEN || peek.type == TOKEN_FI)
            return PARSER_OK;
        
        struct ast *tmp_ast = NULL;
        //Else we need to parse 'and_or'
        if (parser_and_or(lex, &tmp_ast) == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free(tmp_ast);
            return PARSER_UNEXPECTED_TOKEN;
        }
    
        //If and_or works, add command retrieving
        ast_add_brother(*res, tmp_ast);
        peek = lexer_peek(lex);
    }
    return PARSER_OK;
}