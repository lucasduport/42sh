#include "parser.h"

enum parser_status parser_rule_if(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

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
            lexer_pop(lex);
            return PARSER_OK;
        }
    }
    return PARSER_UNEXPECTED_TOKEN;
}

enum parser_status parser_else_clause(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);
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
