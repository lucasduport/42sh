# 42sh

This project is a posix shell written in C
A kind of equivalent to `bash --posix`

## Prerequisites

Make sure you have the following tools installed on your system:
- Autoconf
- Automake
- Make

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

You may want to configure a python virutal environment
```bash
  python3 -m venv .venv
  source .venv/bin/activate
```

You can now install required libraries:
```bash
  pip3 install termcolor pyyaml
```

```bash
  make check
```

## Authors

- [Mathilde Copin](mathilde.copin@epita.fr)
- [Corentin Belloni](corentin.belloni@epita.fr)
- [Flavien Geoffray](flavien.geoffray@epita.fr)
- [Lucas Duport](lucas.duport@epita.fr)
