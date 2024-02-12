## ⚠️ Disclaimer: School Project for Open Source and Educational Purposes

This repository contains code and materials from a school project created for educational purposes. While I encourage the open-source community to explore, learn, and potentially build upon this work, please be aware of the following:

1. **Educational Intent:** This project was developed as a part of my academic curriculum, and its primary purpose is to demonstrate my understanding of the subject matter.

2. **No Guarantee of Suitability:** The project might have limitations, and it may not be optimized for production use. Use it at your own discretion.

3. **No Ongoing Support:** I am not obligated to provide support, updates, or modifications to this project. It is no longer actively maintained.

4. **Not Responsible for Reuse:** If you choose to reuse or modify this work, it is at your own risk. I am not responsible for any consequences resulting from the reuse of this project.

5. **Attribution:** If you decide to use or adapt this project, please consider giving proper attribution.

Remember, this repository is intended to foster learning and collaboration within the open-source and educational communities. Enjoy exploring and happy coding! 🚀

This project is a posix shell written in C
A kind of equivalent to `bash --posix`

# 42sh

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
