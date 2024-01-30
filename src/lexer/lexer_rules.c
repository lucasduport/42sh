#include "lexer.h"

int preprocess(struct lexer *lexer)
{
    // We tract backslash char by pushing it on the stack to process
    // it when the next char i
    if (stack_peek(lexer->mode_stack) == BACKSLASH)
    {
        stack_pop(&lexer->mode_stack);
        if (lexer->current_char != '\'')
        {
            feed(lexer->current_word, lexer->current_char);
            return 1;
        }
    }
    // We process dollar by pushing it and then we have either $( or ${
    else if (stack_peek(lexer->mode_stack) == DOLLAR)
    {
        stack_pop(&lexer->mode_stack);
        if (lexer->current_char == '(')
        {
            stack_push(&lexer->mode_stack, DOLLAR_PAR);
            feed(lexer->current_word, lexer->current_char);
            return 1;
        }
        else if (lexer->current_char == '{')
        {
            stack_push(&lexer->mode_stack, DOLLAR_BRACE);
            feed(lexer->current_word, lexer->current_char);
            return 1;
        }
    }
    return 0;
}

struct token process_rule_one(struct lexer *lexer)
{
    if (lexer->current_word->len == 0)
        feed(lexer->current_word, lexer->current_char);

    if (!is_empty(lexer->mode_stack))
    {
        stack_destroy(lexer->mode_stack);
        lexer->mode_stack = NULL;
        return token_alloc(TOKEN_ERROR, TOKEN_FAM_WORD, lexer);
    }

    return token_new(lexer);
}

struct token process_rule_three(struct lexer *lexer)
{
    lexer->last_is_op = 0;

    struct token tok = token_new(lexer);
    if (!isblank(lexer->current_char))
    {
        feed(lexer->current_word, lexer->current_char);
        if (first_char_op(lexer))
            lexer->last_is_op = 1;
        else if (is_quote_char(lexer))
            process_rule_four(lexer);
    }

    return tok;
}

void process_rule_four(struct lexer *lexer)
{
    enum quote_type peek = stack_peek(lexer->mode_stack);

    if (lexer->current_char == '\\' && peek != SINGLE_Q)
        stack_push(&lexer->mode_stack, BACKSLASH);

    // Open or close quoting mode
    if (lexer->current_char == '"' && peek != SINGLE_Q)
    {
        if (stack_peek(lexer->mode_stack) == DOUBLE_Q)
            stack_pop(&lexer->mode_stack);
        else
            stack_push(&lexer->mode_stack, DOUBLE_Q);
    }

    if (lexer->current_char == '\'' && peek != DOUBLE_Q)
    {
        if (stack_peek(lexer->mode_stack) == SINGLE_Q)
            stack_pop(&lexer->mode_stack);
        else
            stack_push(&lexer->mode_stack, SINGLE_Q);
    }
}

void process_rule_five(struct lexer *lexer)
{
    if (lexer->current_char == '`')
    {
        if (stack_peek(lexer->mode_stack) == BACKTICK)
            stack_pop(&lexer->mode_stack);
        else
            stack_push(&lexer->mode_stack, BACKTICK);
    }

    else
        stack_push(&lexer->mode_stack, DOLLAR);
}

int process_rule_six(struct lexer *lexer)
{
    // Parentheses mode or nothing
    if (stack_peek(lexer->mode_stack) == PAR
        || stack_peek(lexer->mode_stack) == EMPTY)
    {
        // If the token is a right parenthese and peeek = ( => pop (close)
        if (stack_peek(lexer->mode_stack) == PAR && lexer->current_char == ')')
            stack_pop(&lexer->mode_stack);

        lexer->last_is_op = 1;

        if (lexer->current_word->len != 0)
            return 1;
        else
            feed(lexer->current_word, lexer->current_char);
    }
    else
    {
        // Quoting mode
        feed(lexer->current_word, lexer->current_char);
        enum quote_type peek = stack_peek(lexer->mode_stack);
        if (lexer->current_char == '(')
        {
            if (peek == DOLLAR_PAR || peek == DOLLAR_BRACE || peek == BACKTICK)
                stack_push(&lexer->mode_stack, PAR_IGNORE);
        }

        // Close subshell
        else if (lexer->current_char == ')'
                 && (peek == DOLLAR_PAR || peek == PAR_IGNORE))
            stack_pop(&lexer->mode_stack);
    }
    return 0;
}
