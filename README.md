# compiler

A subset-of-C compiler that translates source code into Portuguese natural language descriptions, built for the Compilers course.

[![License: MIT](https://img.shields.io/badge/License-MIT-3da639.svg)](LICENSE)
![Status](https://img.shields.io/badge/status-completed-6f42c1)

[![C](https://img.shields.io/badge/C-00599C?logo=c&logoColor=white)](https://en.cppreference.com/w/c)
[![Make](https://img.shields.io/badge/Make-4495D5?logo=make&logoColor=white)](https://www.gnu.org/software/make/)

[Portuguese](README.pt.md) | English

## About

Implementation of a compiler front-end that reads a subset of C (functions, variables, control flow, arithmetic) and outputs a natural language description of the program in Portuguese. Built with Flex (lexer) and Bison (parser), producing an AST that is then walked by a translator.

## Features

| Component | Description |
| --------- | ----------- |
| **Lexer** | Tokenises keywords, identifiers, operators, and constants via Flex. |
| **Parser** | Builds an AST from the token stream using a Bison grammar. |
| **AST** | Tree structure representing the parsed program (functions, statements, expressions). |
| **Translator** | Walks the AST and emits a Portuguese natural language description. |

## Supported subset of C

| Feature | Examples |
| ------- | -------- |
| Types | `int`, `float`, `char`, `double`, `void` |
| Declarations & assignments | `int x = 5;`, `x += 3;` |
| Control flow | `if/else`, `switch/case`, `while`, `do-while`, `for` |
| Functions | definitions with parameters and return values |
| Expressions | arithmetic, comparison, bitwise, logical, ternary |

## Requirements

| Tool  | Minimum version |
| ----- | --------------- |
| GCC   | 9+              |
| Make  | 4+              |
| Flex  | 2.6+            |
| Bison | 3.0+            |

Linux only — native macOS execution is not supported.

## How to run

```bash
make                        # compile the compiler
./bin/compiler input.c      # translate input.c to Portuguese
./bin/compiler < input.c    # read from stdin
make clean                  # remove build artifacts
```

Test files are in `tests/`. Output is written to stdout.

## License

Distributed under the **MIT** license, © 2024 Nycolas Souza.

It is a permissive license: anyone can use, copy, modify, and distribute the code, including in commercial projects, as long as the copyright notice and license text are retained.

The full text is in [LICENSE](LICENSE).
