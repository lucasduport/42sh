#include "lexer.h"

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

// ---------------------- TOKENIZER ------------------------
struct token token_alloc(enum token_type type, enum token_family family,
                         struct lexer *lexer)
{
    struct token token;
    token.data = calloc(lexer->current_word->len, sizeof(char));
    token.data =
        memcpy(token.data, lexer->current_word->data, lexer->current_word->len);
    token.type = type;
    token.family = family;

    string_reset(lexer->current_word);

    // print_token(token);
    return token;
}

struct token token_new(struct lexer *lexer)
{
    feed(lexer->current_word, '\0');

    char *reserved_words[] = { "if",   "then",  "elif",  "else", "fi", "do",
                               "done", "while", "until", "for",  "in", "!",
                               "case", "esac",  "{",     "}",    ";",  "\n",
                               "|",    "&&",    "||",    ";;",   "\0", "(",
                               ")",    "<",     ">",     "<<",   ">>", "<&",
                               ">&",   "<>",    ">|" };

    int family = 0;
    for (size_t i = 0; i < sizeof(reserved_words) / sizeof(char *); i++)
    {
        if (i == 16 || i == 25)
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

static int check_comment(struct lexer *lexer)
{
    return !stack_quoted(lexer->mode_stack)
        && ((lexer->current_char == '#' && lexer->current_word->len == 0)
            || (lexer->current_word->len == 1
                && lexer->current_word->data[0] == '#'));
}

struct token tokenizer(struct lexer *lexer)
{
    lexer->current_char = io_getchar();

    // ---------------------------- PREPROCESS ---------------------------
    if (preprocess(lexer))
        return tokenizer(lexer);

    // ---------------------------- RULE 1 ---------------------------
    if (lexer->current_char == '\0')
        return process_rule_one(lexer);

    // ---------------------------- RULE 2 ---------------------------
    else if (lexer->last_is_op && is_empty(lexer->mode_stack)
             && is_valid_operator(lexer))
        feed(lexer->current_word, lexer->current_char);

    // ---------------------------- RULE 3 ---------------------------
    else if (lexer->last_is_op && !is_valid_operator(lexer))
        return process_rule_three(lexer);

    // ---------------------------- RULE 4 ---------------------------
    else if (is_quote_char(lexer))
    {
        feed(lexer->current_word, lexer->current_char);
        process_rule_four(lexer);
    }

    // ---------------------------- RULE 5 ---------------------------
    else if (is_sub_char(lexer) && stack_peek(lexer->mode_stack) != SINGLE_Q
             && stack_peek(lexer->mode_stack) != DOUBLE_Q)
    {
        // Append '$' or '`'
        feed(lexer->current_word, lexer->current_char);
        process_rule_five(lexer);
    }

    // ---------------------------- RULE 6 ---------------------------
    else if (first_char_op(lexer))
    {
        if (process_rule_six(lexer))
        {
            struct token tok = token_new(lexer);
            feed(lexer->current_word, lexer->current_char);

            return tok;
        }
    }

    // ---------------------------- RULE 7 ---------------------------
    else if (check_comment(lexer))
        return skip_comment(lexer);

    // ---------------------------- RULE 8 ---------------------------
    else if ((stack_peek(lexer->mode_stack) == PAR
              || stack_peek(lexer->mode_stack) == EMPTY)
             && isblank(lexer->current_char))
    {
        if (lexer->current_word->len != 0)
            return token_new(lexer);
    }

    // ---------------------------- RULE 9 ---------------------------
    else
    {
        feed(lexer->current_word, lexer->current_char);
        if (lexer->current_char == '}'
            && stack_peek(lexer->mode_stack) == DOLLAR_BRACE)
            stack_pop(&lexer->mode_stack);
    }

    return tokenizer(lexer);
}
