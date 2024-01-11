#include "parser/parser.h"

int main(int argc, char **argv)
{
    struct ast *res;
    if (parser(argc, argv, &res) != PARSER_OK)
    {
        debug_printf("42sh: ast creation failed\n");
        return 2;
    }

    // TODO: Ast evalutation.

    return 0;
}
