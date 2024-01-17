#ifndef REDIR_H
#define REDIR_H

struct redirection
{
    int word_fd;
    int io_number;
};

int redir(char *filename, char *io_number, char *operator);

#endif /* ! REDIR_H */
