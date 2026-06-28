// ============================================================================
// MAIN - Compiler entry point
// ============================================================================
// Flow:
// 1. Open input file
// 2. Run the parser (which calls the lexer internally)
// 3. On success, translate the AST to natural language
// 4. Write the result to a file or stdout

#include "ast.h"
#include "translator.h"
#include <stdio.h>
#include <stdlib.h>

// External declarations (defined by Flex and Bison)
extern FILE* yyin;    // input file for the lexer
extern int yyparse(); // main parser function
extern ASTNode* root; // AST root (set by the parser)

// ============================================================================
// int main - Main function
// ============================================================================
// argc: command-line argument count
// argv: argument array
//   argv[0]: program name
//   argv[1]: input file (required)
//   argv[2]: output file (optional, defaults to stdout)
int main(int argc, char** argv)
{
    // check arguments: at least an input file is required
    if (argc < 2)
    {
        fprintf(stderr, "Uso: %s <arquivo_entrada> [arquivo_saida]\n", argv[0]);
        return 1;
    }

    // ========================================================================
    // STEP 1: Open input file
    // ========================================================================
    FILE* in = fopen(argv[1], "r");
    if (!in)
    {
        perror("Erro ao abrir arquivo de entrada");
        return 1;
    }
    yyin = in; // set input file for the lexer

    // ========================================================================
    // STEP 2: Parser — read and analyse tokens
    // ========================================================================
    // yyparse() calls yylex() internally to obtain tokens
    // returns 0 on success, non-zero on error
    if (yyparse() != 0)
    {
        fprintf(stderr, "Erro durante o parsing.\n");
        fclose(in);
        return 1;
    }

    // ========================================================================
    // STEP 3: Open output file
    // ========================================================================
    FILE* out = stdout; // default: write to stdout
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
    // STEP 4: Translate and clean up
    // ========================================================================
    if (root)
    {
        // translate the AST to natural language
        translate_ast(root, out);
        // free AST memory
        free_ast(root);
    }

    // close files
    fclose(in);
    if (out != stdout)
        fclose(out);

    return 0; // success
}
