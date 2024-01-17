# 42sh

Implement an equivalent of /usr/bin/env bash --posix

## Prerequisites

Make sure you have the following tools installed on your system:

- Autoconf
- Automake
- Make

If you are on PIE, you have to configure a venv
```bash
  python3 -m venv .venv
  export .venv/bin/activate
  which python # It should print the new path of your python interpreter
```

You can now install required libraries:
```bash
  pip3 install termcolor pyyaml
```

## Run Locally

Clone the project

```bash
  git clone xavier.login@git.forge.epita.fr:p/epita-ing-assistants-acu/42sh-2026/epita-ing-assistants-acu-42sh-2026-lyon-21.git
```

Go to the project directory

```bash
  cd epita-ing-assistants-acu-42sh-2026-lyon-21
```

Configure autotools

```bash
  autoreconf -i
```

Generate Makefiles

```bash
  ./configure [--enable-debug | --enable-coverage]
```

Make and run !

```bash
  make && src/42sh [-c "<input string>" | <shell script>]
```
## Running Tests

To run tests, run the following command

```bash
  make check
```

## Appendix

Any additional information goes here

## Historic

Write historic here


## Currently

IO_backend:
  Manages IO_backend so that no matter the type of input, the lexer always retrieves the same thing.
  Return '\0' for a end of file no matter the type of input

Lexer:
  Reads the input character by character using the IO_backend and returns a token.
  For now returns:
    TOKEN_NEWLINE for a newline
    TOKEN_IF
    TOKEN_THEN
    TOKEN_ELIF
    TOKEN_ELSE
    TOKEN_FI
    TOKEN_SEMICOLONS
    TOKEN_EOF for end of input
    TOKEN_WORD for anything else

Parser:
  Constructs the AST for execution. For the moment only manages grammar that can generate 'if then elif /else' and command lists.

Executable:
  Executes the AST corresponding to the entry.
  Only supports 'echo', 'true' and 'false' builtins.
  Returns the same values ​​as executing in a classic bash shell. Displays error messages.

Logger:
  Allows you to set a debug mode.
  To do this, you must initialize the logger in main function. If it is not initialized all debug displays present in the code have no effect.
  It can be called from anywhere in the code.

## Authors

- [Mathilde Copin](mathilde.copin@epita.fr)
- [Corentin Belloni](corentin.belloni@epita.fr)
- [Flavien Geoffray](flavien.geoffray@epita.fr)
- [Lucas Duport](lucas.duport@epita.fr)
