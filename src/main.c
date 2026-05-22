// ============================================================================
// MAIN - Ponto de entrada do compilador
// ============================================================================
// Fluxo:
// 1. Abre arquivo de entrada
// 2. Chama o parser (que usa o lexer internamente)
// 3. Se bem-sucedido, traduz a AST para português
// 4. Escreve resultado em arquivo ou stdout

#include "ast.h"
#include "translator.h"
#include <stdio.h>
#include <stdlib.h>

// Declarações externas (definidas pelo Flex e Bison)
extern FILE* yyin;       // Arquivo de entrada para o lexer
extern int yyparse();    // Função principal do parser
extern ASTNode* root;    // Raiz da AST (definida pelo parser)

// ============================================================================
// int main - Função principal
// ============================================================================
// argc: número de argumentos de linha de comando
// argv: array de strings com os argumentos
//   argv[0]: nome do programa
//   argv[1]: arquivo de entrada (obrigatório)
//   argv[2]: arquivo de saída (opcional, padrão: stdout)
int main(int argc, char** argv)
{
    // Verifica argumentos: precisa de pelo menos arquivo de entrada
    if (argc < 2)
    {
        fprintf(stderr, "Uso: %s <arquivo_entrada> [arquivo_saida]\n", argv[0]);
        return 1;
    }

    // ========================================================================
    // ETAPA 1: Abre arquivo de entrada
    // ========================================================================
    FILE* in = fopen(argv[1], "r");
    if (!in)
    {
        perror("Erro ao abrir arquivo de entrada");
        return 1;
    }
    yyin = in;  // Define o arquivo de entrada para o lexer

    // ========================================================================
    // ETAPA 2: Parser - Lê e analisa tokens
    // ========================================================================
    // yyparse() chama yylex() internamente para obter tokens
    // Se sucesso: retorna 0, se erro: retorna não-zero
    if (yyparse() != 0)
    {
        fprintf(stderr, "Erro durante o parsing.\n");
        fclose(in);
        return 1;
    }

    // ========================================================================
    // ETAPA 3: Abre arquivo de saída
    // ========================================================================
    FILE* out = stdout;  // Padrão: escreve em stdout
    if (argc >= 3)
    {
        out = fopen(argv[2], "w");
        if (!out)
        {
            perror("Erro ao abrir arquivo de saída");
            fclose(in);
            return 1;
        }
    }

    // ========================================================================
    // ETAPA 4: Tradução e Limpeza
    // ========================================================================
    if (root)
    {
        // Traduz a AST (converter para português)
        translate_ast(root, out);
        // Libera a memória da AST
        free_ast(root);
    }

    // Fecha arquivos
    fclose(in);
    if (out != stdout)
        fclose(out);

    return 0;  // Sucesso
}
