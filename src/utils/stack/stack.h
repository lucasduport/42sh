#ifndef STACK_H
#define STACK_H

enum quote_type
{
    EMPTY,
    DOUBLE_Q,
    SINGLE_Q,
    DOLLAR_PAR,
    PAR,
    DOLLAR,
    BACKTICK,
    DOLLAR_BRACE,
    BACKSLASH
};

struct stack
{
    enum quote_type data;
    struct stack *next;
};

void stack_push(struct stack **s, enum quote_type q);
enum quote_type stack_pop(struct stack **s);
enum quote_type stack_peek(struct stack *s);
int stack_destroy(struct stack *s);
int is_empty(struct stack *s);

#endif /* !STACK_H */
