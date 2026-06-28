# compiler

Um compilador de subconjunto de C que traduz código fonte para descrições em linguagem natural em português, construído para a disciplina de Compiladores.

[![License: MIT](https://img.shields.io/badge/License-MIT-3da639.svg)](LICENSE)
![Status](https://img.shields.io/badge/status-concluído-6f42c1)

[![C](https://img.shields.io/badge/C-00599C?logo=c&logoColor=white)](https://en.cppreference.com/w/c)
[![Make](https://img.shields.io/badge/Make-4495D5?logo=make&logoColor=white)](https://www.gnu.org/software/make/)

Português | [English](README.md)

## Sobre

Implementação de um front-end de compilador que lê um subconjunto de C (funções, variáveis, fluxo de controle, aritmética) e gera uma descrição em linguagem natural do programa em português. Construído com Flex (léxico) e Bison (parser), produzindo uma AST percorrida por um tradutor.

## Funcionalidades

| Componente | Descrição |
| ---------- | --------- |
| **Léxico** | Tokeniza palavras-chave, identificadores, operadores e constantes via Flex. |
| **Parser** | Constrói uma AST a partir do fluxo de tokens usando uma gramática Bison. |
| **AST** | Estrutura de árvore que representa o programa analisado (funções, statements, expressões). |
| **Tradutor** | Percorre a AST e emite uma descrição em linguagem natural em português. |

## Subconjunto de C suportado

| Recurso | Exemplos |
| ------- | -------- |
| Tipos | `int`, `float`, `char`, `double`, `void` |
| Declarações e atribuições | `int x = 5;`, `x += 3;` |
| Fluxo de controle | `if/else`, `switch/case`, `while`, `do-while`, `for` |
| Funções | definições com parâmetros e valores de retorno |
| Expressões | aritmética, comparação, bitwise, lógica, ternária |

## Requisitos

| Ferramenta | Versão mínima |
| ---------- | ------------- |
| GCC        | 9+            |
| Make       | 4+            |
| Flex       | 2.6+          |
| Bison      | 3.0+          |

Apenas Linux — execução nativa no macOS não é suportada.

## Como executar

```bash
make                        # compilar o compilador
./bin/compiler input.c      # traduzir input.c para português
./bin/compiler < input.c    # ler da entrada padrão
make clean                  # remover artefatos de build
```

Arquivos de teste estão em `tests/`. A saída é escrita na saída padrão.

## Licença

Distribuído sob a licença **MIT**, © 2024 Nycolas Souza.

É uma licença permissiva: qualquer pessoa pode usar, copiar, modificar e distribuir o código, inclusive em projetos comerciais, desde que o aviso de copyright e o texto da licença sejam mantidos.

O texto completo está em [LICENSE](LICENSE).
