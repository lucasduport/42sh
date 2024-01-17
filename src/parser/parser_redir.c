#include "parser.h"

enum parser_status parser_pipeline(struct lexer *lex, struct ast **res)
{
    struct token peek = lexer_peek(lex);

    // Catch negation if there is
    struct ast *tmp_final = NULL;
    if (peek.type == TOKEN_NEG)
    {
        tmp_final= ast_new(AST_NEG);
        token_free(lexer_pop(lex));
        peek = lexer_peek(lex);
    }

    if (parser_command(lex, res) == PARSER_UNEXPECTED_TOKEN)
    {
        debug_printf(LOG_PARS, "[PARSER] Failed parse first command - pipeline\n");
        ast_free(tmp_final);
        return PARSER_UNEXPECTED_TOKEN;
    }
    if (tmp_final != NULL)
        tmp_final->first_child = *res;
    else
        tmp_final = *res;
    
    // Parse optional { '|' {'\n'} command }
    peek = lexer_peek(lex);
    while (peek.type == TOKEN_PIPE)
    {
        struct ast *tmp_pipe = ast_new(AST_PIPE);
        token_free(lexer_pop(lex));
        peek = lexer_peek(lex);

        // Skip optional newline
        while (peek.type == TOKEN_NEWLINE)
        {
            token_free(lexer_pop(lex));
            peek = lexer_peek(lex);
        }

        if (parser_command(lex, res) == PARSER_UNEXPECTED_TOKEN)
        {
            ast_free(tmp_pipe);
            ast_free(tmp_final);
            return PARSER_UNEXPECTED_TOKEN;
        }

        // Create pipeline AST
        tmp_pipe->first_child = tmp_final;
        ast_add_brother(tmp_final, *res);
        tmp_final = tmp_pipe;

        peek = lexer_peek(lex);
    }

    *res = tmp_final;
    return PARSER_OK;
}

//TODO Implement prefix
//TODO Implement redirection