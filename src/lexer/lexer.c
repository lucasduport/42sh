#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif /* ! _POSIX_C_SOURCE */

#include "lexer.h"

/**
 * @brief Creates a new token
 *
 * @param type The type of the token
 * @param lexer
 * @return The new token
 */
static struct token token_alloc(enum token_type type, enum token_family family,
                                struct lexer *lexer)
{
    struct token token;
    token.data = calloc(lexer->current_word->len, sizeof(char));
    token.data =
        memcpy(token.data, lexer->current_word->data, lexer->current_word->len);
    token.type = type;
    token.family = family;

    string_reset(lexer->current_word);

    print_token(token);
    return token;
}

/**
 * @brief Check if the current word could be an IO number
 *
 * @param lexer The lexer
 */
static int check_io_number(struct lexer *lexer)
{
    for (size_t i = 0; lexer->current_word->data[i] != '\0'; i++)
    {
        if (!isdigit(lexer->current_word->data[i]))
            return 0;
    }

    return lexer->current_char == '<' || lexer->current_char == '>';
}

static int check_special_variable(const char *name)
{
    char special_char[] = { '$', '?', '@', '*', '#', '!', '.', '-', '+' };

    for (size_t i = 0; name[i] != '\0'; i++)
    {
        for (size_t j = 0; j < sizeof(special_char) / sizeof(char); j++)
        {
            if (name[i] == special_char[j])
                return 1;
        }
    }
    return 0;
}

/**
 * @brief Check if the current word could be an assignment word
 *
 * a=2 ?
 *
 * @param lexer The lexer
 */
static int check_assignment(struct lexer *lexer)
{
    int contains_equal = 0;
    for (size_t i = 0; i < lexer->current_word->len; i++)
    {
        if (lexer->current_word->data[i] == '=')
            contains_equal++;
    }

    if (contains_equal >= 1)
    {
        if (lexer->current_word->data[0] == '=')
            return 0;

        char *word_cpy = strdup(lexer->current_word->data);
        char *variable_name = strtok(word_cpy, "=");

        int code = 0;

        if (variable_name == NULL)
            code = 0;

        else if (check_special_variable(variable_name))
            code = 0;
        else
            code = 1;

        free(word_cpy);
        return code;
    }
    return 0;
}

/**
 * @brief check if the word is a reserved word
 *
 * @param word
 * @return struct token
 */
static struct token token_new(struct lexer *lexer)
{
    feed(lexer->current_word, '\0');

    char *reserved_words[] = { "if",   "then",  "elif",  "else", "fi", "do",
                               "done", "while", "until", "for",  "in", "!",
                               "{",    "}",     ";",     "\n",   "|",  "&&",
                               "||",   ";;",    "\0",    "(",    ")",  "<",
                               ">",    "<<",    ">>",    "<&",   ">&", "<>",
                               ">|" };

    int family = 0;
    for (size_t i = 0; i < sizeof(reserved_words) / sizeof(char *); i++)
    {
        // debug_printf(LOG_LEX,"test '%s' == '%s'\n", reserved_words[i],
        //              lexer->current_word->data);

        if (i == 14 || i == 23)
            family++;

        if (!strcmp(reserved_words[i], lexer->current_word->data))
        {
            return token_alloc((enum token_type)i, (enum token_family)family,
                               lexer);
        }
    }

    if (check_io_number(lexer))
        return token_alloc(TOKEN_WORD, TOKEN_FAM_IO_NUMBER, lexer);

    if (check_assignment(lexer))
        return token_alloc(TOKEN_WORD, TOKEN_FAM_ASSW, lexer);

    return token_alloc(TOKEN_WORD, TOKEN_FAM_WORD, lexer);
}

/**
 *  if (lexer->current_char == '\0')
 */
static struct token process_rule_one(struct lexer *lexer)
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

/**
 *  if (lexer->last_is_op && !is_valid_operator(lexer))
 */
static struct token process_rule_three(struct lexer *lexer)
{
    lexer->last_is_op = 0;

    struct token tok = token_new(lexer);
    if (!isblank(lexer->current_char))
        feed(lexer->current_word, lexer->current_char);

    return tok;
}

static int stack_quoted(struct lexer *lexer)
{
    enum quote_type q = stack_peek(lexer->mode_stack);
    return q == DOUBLE_Q || q == SINGLE_Q;
}

/**
 * @brief The token recognition algorithm described in the SCL
 *
 * @param lexer
 * @return struct token
 */
static struct token tokenizer(struct lexer *lexer)
{
    lexer->current_char = io_getchar();

    // We tract backslash char by pushing it on the stack to process
    // it when the next char i
    if (stack_peek(lexer->mode_stack) == BACKSLASH)
    {
        stack_pop(&lexer->mode_stack);
        if (lexer->current_char != '\'')
        {
            feed(lexer->current_word, lexer->current_char);
            return tokenizer(lexer);
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
            return tokenizer(lexer);
        }
        else if (lexer->current_char == '{')
        {
            stack_push(&lexer->mode_stack, DOLLAR_BRACE);
            feed(lexer->current_word, lexer->current_char);
            return tokenizer(lexer);
        }
    }

    // -------------------------------------------------------------------------
    // --------------------------------- RULE 1 --------------------------------
    // -------------------------------------------------------------------------
    if (lexer->current_char == '\0')
        return process_rule_one(lexer);

    // -------------------------------------------------------------------------
    // --------------------------------- RULE 2 --------------------------------
    // -------------------------------------------------------------------------
    else if (lexer->last_is_op && is_empty(lexer->mode_stack)
             && is_valid_operator(lexer))
        feed(lexer->current_word, lexer->current_char);

    // -------------------------------------------------------------------------
    // --------------------------------- RULE 3 --------------------------------
    // -------------------------------------------------------------------------
    else if (lexer->last_is_op && !is_valid_operator(lexer))
        return process_rule_three(lexer);

    // -------------------------------------------------------------------------
    // --------------------------------- RULE 4 --------------------------------
    // -------------------------------------------------------------------------
    else if (is_quote_char(lexer))
    {
        feed(lexer->current_word, lexer->current_char);
        if (lexer->current_char == '\\')
        {
            // BACKLASH will allow next char to be escaped if needed
            stack_push(&lexer->mode_stack, BACKSLASH);
        }
        // Open or close quoting mode
        else if (lexer->current_char == '"')
        {
            if (stack_peek(lexer->mode_stack) == DOUBLE_Q)
                stack_pop(&lexer->mode_stack);
            else
                stack_push(&lexer->mode_stack, DOUBLE_Q);
        }
        else if (lexer->current_char == '\'')
        {
            if (stack_peek(lexer->mode_stack) == SINGLE_Q)
                stack_pop(&lexer->mode_stack);
            else
                stack_push(&lexer->mode_stack, SINGLE_Q);
        }
    }

    // -------------------------------------------------------------------------
    // --------------------------------- RULE 5 --------------------------------
    // -------------------------------------------------------------------------
    else if (is_sub_char(lexer) && stack_peek(lexer->mode_stack) != SINGLE_Q)
    {
        // Append '$' or '`'
        feed(lexer->current_word, lexer->current_char);
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

    // -------------------------------------------------------------------------
    // --------------------------------- RULE 6 --------------------------------
    // -------------------------------------------------------------------------
    else if (first_char_op(lexer))
    {
        // Parentheses mode or nothing
        if (stack_peek(lexer->mode_stack) == PAR || stack_peek(lexer->mode_stack) == EMPTY)
        {
            // If the token is a right parenthese and peeek = ( => pop (closse)
            if (stack_peek(lexer->mode_stack) == PAR && lexer->current_char == ')')
                stack_pop(&lexer->mode_stack);

            lexer->last_is_op = 1;

            if (lexer->current_word->len != 0)
            {
                struct token tok = token_new(lexer);
                feed(lexer->current_word, lexer->current_char);

                // lexer->is_newline = lexer->current_char == '\n';

                return tok;
            }
            else
                feed(lexer->current_word, lexer->current_char);
        }
        else
        {
            // Quoting mode
            feed(lexer->current_word, lexer->current_char);
            // Close subshell
            if (lexer->current_char == ')'&& stack_peek(lexer->mode_stack) == DOLLAR_PAR)
                    stack_pop(&lexer->mode_stack);
        }
    }

    else if (!stack_quoted(lexer) && ((lexer->current_char == '#' && lexer->current_word->len == 0) || (lexer->current_word->len == 1 && lexer->current_word->data[0] == '#')))
        return skip_comment(lexer);

    // -------------------------------------------------------------------------
    // --------------------------------- RULE 7 --------------------------------
    // -------------------------------------------------------------------------
    else if ((stack_peek(lexer->mode_stack) == PAR || stack_peek(lexer->mode_stack) == EMPTY) && isblank(lexer->current_char))
    {
        if (lexer->current_word->len != 0)
            return token_new(lexer);
    }

    // -------------------------------------------------------------------------
    // --------------------------------- RULE 8 --------------------------------
    // -------------------------------------------------------------------------
    // else if (lexer->current_word->len != 0)
    //{
    //    feed(lexer->current_word, lexer->current_char);
    //    check_special_behavior(lexer);
    //}

    // -------------------------------------------------------------------------
    // --------------------------------- RULE 9 --------------------------------
    // -------------------------------------------------------------------------
    else
    {
        feed(lexer->current_word, lexer->current_char);
        if (lexer->current_char == '}' && stack_peek(lexer->mode_stack) == DOLLAR_BRACE)
            stack_pop(&lexer->mode_stack);
    }

    return tokenizer(lexer);
}

struct lexer *lexer_new(int argc, char *argv[])
{
    if (io_abstraction(argc, argv) == IO_FAILED)
        return NULL;

    struct lexer *lexer = calloc(1, sizeof(struct lexer));
    lexer->last_token = token_null();
    lexer->current_word = string_create();
    return lexer;
}

struct token lexer_peek(struct lexer *lexer)
{
    if (lexer->last_token.type != TOKEN_NULL)
        return lexer->last_token;

    struct token tok = tokenizer(lexer);
    lexer->last_token = tok;

    return tok;
}

struct token lexer_pop(struct lexer *lexer)
{
    if (lexer->last_token.type != TOKEN_NULL)
    {
        struct token tok = lexer->last_token;
        lexer->last_token = token_null();
        return tok;
    }

    return tokenizer(lexer);
}

void lexer_free(struct lexer *lexer)
{
    string_destroy(lexer->current_word);
    stack_destroy(lexer->mode_stack);
    free(lexer);
}
