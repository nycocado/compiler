// ============================================================================
// TRANSLATOR - Traduz AST para Português
// ============================================================================
// Converte a árvore de sintaxe abstrata em descrições em português natural.
// Exemplo: x + 5 → "a soma de x e 5"

#include "translator.h"
#include <stdio.h>
#include <string.h>

// Declaração antecipada (usada recursivamente)
void translate_expression(ASTNode* node, FILE* out);

// ============================================================================
// get_type_pt - Traduz tipo para português (artigo/descrição)
// ============================================================================
// Converte nomes técnicos em descrições em português.
// Exemplos:
//   "int" → "inteira"
//   "float" → "real"
//   "void" → "vazio"
const char* get_type_pt(char* type)
{
    if (strcmp(type, "int") == 0)
        return "inteira";
    if (strcmp(type, "float") == 0)
        return "real";
    if (strcmp(type, "double") == 0)
        return "real de precisão dupla";
    if (strcmp(type, "char") == 0)
        return "caractere";
    if (strcmp(type, "void") == 0)
        return "vazio";
    return type;
}

// ============================================================================
// get_type_noun_pt - Retorna nome do tipo em português (singular/plural)
// ============================================================================
// Adapta gênero e número.
// Exemplos:
//   ("int", 0) → "inteiro"
//   ("int", 1) → "inteiros"
//   ("float", 0) → "real"
const char *get_type_noun_pt(char *type, int plural)
{
    if (strcmp(type, "int") == 0)
        return plural ? "inteiros" : "inteiro";
    if (strcmp(type, "float") == 0)
        return plural ? "reais" : "real";
    if (strcmp(type, "double") == 0)
        return plural ? "reais de precisão dupla" : "real de precisão dupla";
    if (strcmp(type, "char") == 0)
        return plural ? "caracteres" : "caractere";
    return plural ? "valores do tipo" : "um valor do tipo";
}

// ============================================================================
// translate_call - Traduz chamada de função
// ============================================================================
// Exemplo:
//   print(5) → "o resultado da chamada à função print, que recebe o valor 5"
//   add(a, b) → "o resultado da chamada à função add, que recebe os valores a e b"
void translate_call(ASTNode *node, FILE *out)
{
    fprintf(out, "o resultado da chamada à função %s", node->value);

    if (node->left)  // Se há argumentos
    {
        fprintf(out, ", que recebe ");

        // Conta quantos argumentos há
        ASTNode *arg = node->left;
        int count = 0;
        ASTNode *tmp = arg;
        while (tmp)
        {
            count++;
            tmp = tmp->next;
        }

        // Escolhe "valor" ou "valores"
        if (count == 1)
            fprintf(out, "o valor ");
        else
            fprintf(out, "os valores ");

        // Traduz cada argumento, separando com vírgula ou "e"
        while (arg)
        {
            translate_expression(arg, out);
            arg = arg->next;
            if (arg)
            {
                if (arg->next)
                    fprintf(out, ", ");      // Vírgula se há mais argumentos
                else
                    fprintf(out, " e ");     // "e" para o último
            }
        }
    }
}


// ============================================================================
// translate_expression - Traduz expressões para português
// ============================================================================
// Processa nós de expressão e escreve a tradução em português.
// Exemplos:
//   x + 5 → "a soma de x e 5"
//   x > 10 → "x é maior que 10"
//   !x → "a negação lógica de x"
void translate_expression(ASTNode* node, FILE* out)
{
    if (!node)
        return;

    // Processa cada tipo de nó de expressão
    switch (node->type)
    {
        case NODE_IDENTIFIER:
            // Variável ou nome de função: apenas escreve o nome
            fprintf(out, "%s", node->value);
            break;

        case NODE_CONSTANT:
            // Valor literal: apenas escreve o valor
            fprintf(out, "%s", node->value);
            break;

        case NODE_CALL:
            // Chamada de função: delega para translate_call
            translate_call(node, out);
            break;

        case NODE_UNOP:
            if (strcmp(node->op, "-") == 0)
            {
                fprintf(out, "o negativo de ");
                translate_expression(node->left, out);
            }
            else if (strcmp(node->op, "+") == 0)
            {
                translate_expression(node->left, out);
            }
            else if (strcmp(node->op, "!") == 0)
            {
                fprintf(out, "a negação lógica de ");
                translate_expression(node->left, out);
            }
            else if (strcmp(node->op, "~") == 0)
            {
                fprintf(out, "o complemento bit a bit de ");
                translate_expression(node->left, out);
            }
            break;

        case NODE_BINOP:
            // Operador binário (com dois operandos)
            // ============ OPERAÇÕES ARITMÉTICAS ============
            if (strcmp(node->op, "+") == 0)
            {
                fprintf(out, "a soma de ");
                translate_expression(node->left, out);
                fprintf(out, " e ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "-") == 0)
            {
                fprintf(out, "a subtração de ");
                translate_expression(node->left, out);
                fprintf(out, " por ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "*") == 0)
            {
                fprintf(out, "o produto de ");
                translate_expression(node->left, out);
                fprintf(out, " e ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "/") == 0)
            {
                fprintf(out, "a divisão de ");
                translate_expression(node->left, out);
                fprintf(out, " por ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "%") == 0)
            {
                fprintf(out, "o resto da divisão de ");
                translate_expression(node->left, out);
                fprintf(out, " por ");
                translate_expression(node->right, out);
            }
            // ============ OPERAÇÕES DE COMPARAÇÃO ============
            else if (strcmp(node->op, "==") == 0)
            {
                // Igualdade
                translate_expression(node->left, out);
                fprintf(out, " é igual a ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "!=") == 0)
            {
                // Desigualdade
                translate_expression(node->left, out);
                fprintf(out, " é diferente de ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, ">") == 0)
            {
                // Maior que
                translate_expression(node->left, out);
                fprintf(out, " é maior que ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "<") == 0)
            {
                // Menor que
                translate_expression(node->left, out);
                fprintf(out, " é menor que ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, ">=") == 0)
            {
                // Maior ou igual
                translate_expression(node->left, out);
                fprintf(out, " é maior ou igual a ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "<=") == 0)
            {
                // Menor ou igual
                translate_expression(node->left, out);
                fprintf(out, " é menor ou igual a ");
                translate_expression(node->right, out);
            }
            // ============ OPERAÇÕES BITWISE E DE DESLOCAMENTO ============
            else if (strcmp(node->op, "<<") == 0)
            {
                // Deslocamento à esquerda
                fprintf(out, "o deslocamento de ");
                translate_expression(node->left, out);
                fprintf(out, " à esquerda por ");
                translate_expression(node->right, out);
                fprintf(out, " bits");
            }
            else if (strcmp(node->op, ">>") == 0)
            {
                // Deslocamento à direita
                fprintf(out, "o deslocamento de ");
                translate_expression(node->left, out);
                fprintf(out, " à direita por ");
                translate_expression(node->right, out);
                fprintf(out, " bits");
            }
            else if (strcmp(node->op, "&") == 0)
            {
                // AND bit a bit
                fprintf(out, "o AND bit a bit entre ");
                translate_expression(node->left, out);
                fprintf(out, " e ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "|") == 0)
            {
                // OR bit a bit
                fprintf(out, "o OR bit a bit entre ");
                translate_expression(node->left, out);
                fprintf(out, " e ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "^") == 0)
            {
                // XOR bit a bit
                fprintf(out, "o XOR bit a bit entre ");
                translate_expression(node->left, out);
                fprintf(out, " e ");
                translate_expression(node->right, out);
            }
            // ============ OPERADORES LÓGICOS ============
            else if (strcmp(node->op, "&&") == 0)
            {
                // AND lógico
                translate_expression(node->left, out);
                fprintf(out, " e ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "||") == 0)
            {
                // OR lógico
                translate_expression(node->left, out);
                fprintf(out, " ou ");
                translate_expression(node->right, out);
            }
            else
            {
                fprintf(out, "(");
                translate_expression(node->left, out);
                fprintf(out, " %s ", node->op);
                translate_expression(node->right, out);
                fprintf(out, ")");
            }
            break;
        case NODE_CONDITIONAL:
            fprintf(out, "o resultado de: se ");
            translate_expression(node->left, out);
            fprintf(out, " for verdade, então ");
            translate_expression(node->middle, out);
            fprintf(out, ", caso contrário ");
            translate_expression(node->right, out);
            break;
        default:
            fprintf(out, "[expressão]");
    }
}

// ============================================================================
// translate_statement - Traduz statements (instruções)
// ============================================================================
// Processa statements como declarações, atribuições, controles de fluxo.
void translate_statement(ASTNode* node, FILE* out)
{
    if (!node)
        return;

    switch (node->type)
    {
        // ============ DECLARAÇÃO ============
        case NODE_DECLARATION:
            if (node->right)
            {
                fprintf(
                    out,
                    "atribui à variável %s %s ",
                    get_type_pt(node->middle->value),
                    node->value
                );
                translate_expression(node->right, out);
            }
            else
            {
                fprintf(
                    out,
                    "declara a variável %s do tipo %s",
                    node->value,
                    node->middle->value
                );
            }
            break;

        // ============ ATRIBUIÇÃO ============
        case NODE_ASSIGNMENT:
            fprintf(out, "atribui à variável %s ", node->left->value);
            if (strcmp(node->op, "=") == 0)
            {
                // simple assignment, do nothing extra
            }
            else if (strcmp(node->op, "+=") == 0)
            {
                fprintf(out, "o resultado da sua soma com ");
            }
            else if (strcmp(node->op, "-=") == 0)
            {
                fprintf(out, "o resultado da sua subtração por ");
            }
            else if (strcmp(node->op, "*=") == 0)
            {
                fprintf(out, "o resultado do seu produto por ");
            }
            else if (strcmp(node->op, "/=") == 0)
            {
                fprintf(out, "o resultado da sua divisão por ");
            }
            else if (strcmp(node->op, "%=") == 0)
            {
                fprintf(out, "o resultado do resto da sua divisão por ");
            }
            else if (strcmp(node->op, "&=") == 0)
            {
                fprintf(out, "o resultado do seu AND bit a bit com ");
            }
            else if (strcmp(node->op, "|=") == 0)
            {
                fprintf(out, "o resultado do seu OR bit a bit com ");
            }
            else if (strcmp(node->op, "^=") == 0)
            {
                fprintf(out, "o resultado do seu XOR bit a bit com ");
            }
            else if (strcmp(node->op, "<<=") == 0)
            {
                fprintf(out, "o resultado do seu deslocamento à esquerda por ");
            }
            else if (strcmp(node->op, ">>=") == 0)
            {
                fprintf(out, "o resultado do seu deslocamento à direita por ");
            }
            else
            {
                fprintf(
                    out, "o resultado de %s %s ", node->left->value, node->op
                );
            }
            translate_expression(node->right, out);
            break;

        // ============ RETURN (DEVOLVE) ============
        case NODE_RETURN:
            fprintf(out, "devolve ");
            translate_expression(node->left, out);
            break;

        // ============ CONTROLE CONDICIONAL (IF/ELSE) ============
        case NODE_IF:
            fprintf(out, "se ");
            translate_expression(node->left, out);
            fprintf(out, " for verdade, então executa { ");
            translate_ast(node->middle, out);
            fprintf(out, " }");
            if (node->right)
            {
                fprintf(out, " caso contrário executa { ");
                translate_ast(node->right, out);
                fprintf(out, " }");
            }
            break;

        // ============ LOOP WHILE ============
        case NODE_WHILE:
            fprintf(out, "enquanto ");
            translate_expression(node->left, out);
            fprintf(out, " for verdade, executa { ");
            translate_ast(node->right, out);
            fprintf(out, " }");
            break;

        // ============ LOOP DO-WHILE ============
        case NODE_DO_WHILE:
            fprintf(out, "executa { ");
            translate_ast(node->left, out);
            fprintf(out, " } enquanto ");
            translate_expression(node->right, out);
            fprintf(out, " for verdade");
            break;

        // ============ BREAK ============
        case NODE_BREAK:
            fprintf(out, "interrompe o ciclo");
            break;

        // ============ LOOP FOR ============
        case NODE_FOR:
            if (node->left)
            {
                fprintf(out, "começa por ");
                translate_statement(node->left, out);
                fprintf(out, ", e ");
            }
            if (node->middle)
            {
                fprintf(out, "enquanto ");
                translate_expression(node->middle, out);
                fprintf(out, " for verdade, ");
            }
            fprintf(out, "executa { ");
            translate_ast(node->right, out);
            fprintf(out, " }");
            break;

        // ============ SWITCH ============
        case NODE_SWITCH:
            fprintf(out, "analisa o valor de ");
            translate_expression(node->left, out);
            fprintf(out, " e executa: ");
            translate_ast(node->right, out);
            break;

        // ============ CASE (dentro de SWITCH) ============
        case NODE_CASE:
            if (node->left)
            {
                fprintf(out, "se for ");
                translate_expression(node->left, out);
                fprintf(out, ", executa ( ");
            }
            else
            {
                fprintf(out, "em todos os outros casos, executa ( ");
            }
            translate_ast(node->right, out);
            fprintf(out, " )");
            break;

        // ============ BLOCO ============
        case NODE_BLOCK:
            // Traduz os statements dentro do bloco
            translate_ast(node->left, out);
            break;

        // ============ CHAMADA DE FUNÇÃO (como statement) ============
        case NODE_CALL:
            fprintf(out, "chama a função %s", node->value);
            break;

        default:
            break;
    }
}

// ============================================================================
// translate_ast - Traduz toda a árvore de sintaxe abstrata
// ============================================================================
// Ponto de entrada: processa cada nó da AST da raiz até as folhas.
// Atravessa a lista ligada de nós (next) até o final.
void translate_ast(ASTNode* node, FILE* out)
{
    ASTNode* current = node;

    // Processa cada nó na lista ligada
    while (current)
    {
        switch (current->type)
        {
            // ============ FUNÇÃO ============
            case NODE_FUNCTION:
                fprintf(out, "A função %s ", current->value);

                // Traduz parâmetros (se houver)
                if (current->left)
                {
                    fprintf(out, "recebe ");
                    ASTNode* param = current->left;

                    // Conta número de parâmetros
                    int count = 0;
                    ASTNode* tmp = param;
                    while (tmp)
                    {
                        count++;
                        tmp = tmp->next;
                    }

                    // Adapta singular/plural
                    if (count == 1)
                        fprintf(out, "um valor ");
                    else if (count == 2)
                        fprintf(out, "dois valores ");
                    else
                        fprintf(out, "%d valores ", count);

                    // Tipo dos parâmetros
                    fprintf(out, "%s, ", get_type_noun_pt(param->middle->value, count > 1));

                    // Lista nomes dos parâmetros
                    tmp = param;
                    while (tmp)
                    {
                        fprintf(out, "%s", tmp->value);
                        tmp = tmp->next;
                        if (tmp)
                        {
                            if (tmp->next)
                                fprintf(out, ", ");
                            else
                                fprintf(out, " e ");
                        }
                    }
                    fprintf(out, ", e ");
                }

                // Verifica se o corpo é apenas um return (para otimizar tradução)
                int has_return = 0;
                if (current->right && current->right->left)
                {
                    ASTNode* stmt = current->right->left;
                    if (stmt->type == NODE_RETURN && !stmt->next)
                    {
                        has_return = 1;
                    }
                }

                // Traduz tipo de retorno (se não há apenas return)
                if (!has_return)
                {
                    fprintf(out, "devolve %s, ", current->middle->value);
                }

                // Traduz corpo da função (statements)
                if (current->right && current->right->left)
                {
                    ASTNode* stmt = current->right->left;
                    while (stmt)
                    {
                        translate_statement(stmt, out);
                        stmt = stmt->next;
                        if (stmt)
                            fprintf(out, ", ");  // Vírgula entre statements
                        else
                            fprintf(out, ".");   // Ponto final
                    }
                }
                else
                {
                    fprintf(
                        out,
                        "devolve %s e não executa nenhuma ação.",
                        current->middle->value
                    );
                }
                fprintf(out, "\n");  // Nova linha após cada função
                break;

            // ============ OUTROS STATEMENTS EM NÍVEL TOP-LEVEL ============
            case NODE_DECLARATION:
            case NODE_ASSIGNMENT:
            case NODE_RETURN:
            case NODE_IF:
            case NODE_WHILE:
            case NODE_DO_WHILE:
            case NODE_FOR:
            case NODE_SWITCH:
            case NODE_CASE:
            case NODE_CALL:
            case NODE_BLOCK:
                translate_statement(current, out);
                break;

            default:
                break;
        }

        // Avança para o próximo nó na lista ligada
        current = current->next;
    }
}
