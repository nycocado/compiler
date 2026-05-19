#include "ast.h"
#include "translator.h"
#include <stdio.h>
#include <stdlib.h>

extern FILE* yyin;
extern int yyparse();
extern ASTNode* root;

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Uso: %s <arquivo_entrada> [arquivo_saida]\n", argv[0]);
        return 1;
    }

    FILE* in = fopen(argv[1], "r");
    if (!in)
    {
        perror("Erro ao abrir arquivo de entrada");
        return 1;
    }
    yyin = in;

    if (yyparse() != 0)
    {
        fprintf(stderr, "Erro durante o parsing.\n");
        fclose(in);
        return 1;
    }

    FILE* out = stdout;
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

    if (root)
    {
        translate_ast(root, out);
        free_ast(root);
    }

    fclose(in);
    if (out != stdout)
        fclose(out);

    return 0;
}
