#include "translator.h"
#include <stdio.h>
#include <string.h>

void translate_expression(ASTNode* node, FILE* out);

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

void translate_call(ASTNode *node, FILE *out)
{
    fprintf(out, "o resultado da chamada à função %s", node->value);
    if (node->left)
    {
        fprintf(out, ", que recebe ");
        ASTNode *arg = node->left;
        int count = 0;
        ASTNode *tmp = arg;
        while (tmp)
        {
            count++;
            tmp = tmp->next;
        }
        if (count == 1)
            fprintf(out, "o valor ");
        else
            fprintf(out, "os valores ");

        while (arg)
        {
            translate_expression(arg, out);
            arg = arg->next;
            if (arg)
            {
                if (arg->next)
                    fprintf(out, ", ");
                else
                    fprintf(out, " e ");
            }
        }
    }
}


void translate_expression(ASTNode* node, FILE* out)
{
    if (!node)
        return;
    switch (node->type)
    {
        case NODE_IDENTIFIER:
            fprintf(out, "%s", node->value);
            break;
        case NODE_CONSTANT:
            fprintf(out, "%s", node->value);
            break;
        case NODE_CALL:
            translate_call(node, out);
            break;
        case NODE_UNOP:
            if (strcmp(node->op, "!") == 0)
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
            else if (strcmp(node->op, "==") == 0)
            {
                translate_expression(node->left, out);
                fprintf(out, " é igual a ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "!=") == 0)
            {
                translate_expression(node->left, out);
                fprintf(out, " é diferente de ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, ">") == 0)
            {
                translate_expression(node->left, out);
                fprintf(out, " é maior que ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "<") == 0)
            {
                translate_expression(node->left, out);
                fprintf(out, " é menor que ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, ">=") == 0)
            {
                translate_expression(node->left, out);
                fprintf(out, " é maior ou igual a ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "<=") == 0)
            {
                translate_expression(node->left, out);
                fprintf(out, " é menor ou igual a ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "<<") == 0)
            {
                fprintf(out, "o deslocamento de ");
                translate_expression(node->left, out);
                fprintf(out, " à esquerda por ");
                translate_expression(node->right, out);
                fprintf(out, " bits");
            }
            else if (strcmp(node->op, ">>") == 0)
            {
                fprintf(out, "o deslocamento de ");
                translate_expression(node->left, out);
                fprintf(out, " à direita por ");
                translate_expression(node->right, out);
                fprintf(out, " bits");
            }
            else if (strcmp(node->op, "&") == 0)
            {
                fprintf(out, "o AND bit a bit entre ");
                translate_expression(node->left, out);
                fprintf(out, " e ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "|") == 0)
            {
                fprintf(out, "o OR bit a bit entre ");
                translate_expression(node->left, out);
                fprintf(out, " e ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "^") == 0)
            {
                fprintf(out, "o XOR bit a bit entre ");
                translate_expression(node->left, out);
                fprintf(out, " e ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "&&") == 0)
            {
                translate_expression(node->left, out);
                fprintf(out, " e ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "||") == 0)
            {
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

void translate_statement(ASTNode* node, FILE* out)
{
    if (!node)
        return;
    switch (node->type)
    {
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
        case NODE_RETURN:
            fprintf(out, "devolve ");
            translate_expression(node->left, out);
            break;
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
        case NODE_WHILE:
            fprintf(out, "enquanto ");
            translate_expression(node->left, out);
            fprintf(out, " for verdade, executa { ");
            translate_ast(node->right, out);
            fprintf(out, " }");
            break;
        case NODE_DO_WHILE:
            fprintf(out, "executa { ");
            translate_ast(node->left, out);
            fprintf(out, " } enquanto ");
            translate_expression(node->right, out);
            fprintf(out, " for verdade");
            break;
        case NODE_BREAK:
            fprintf(out, "interrompe o ciclo");
            break;
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
        case NODE_SWITCH:
            fprintf(out, "analisa o valor de ");
            translate_expression(node->left, out);
            fprintf(out, " e executa: ");
            translate_ast(node->right, out);
            break;
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
        case NODE_BLOCK:
            translate_ast(node->left, out);
            break;
        case NODE_CALL:
            fprintf(out, "chama a função %s", node->value);
            break;
        default:
            break;
    }
}

void translate_ast(ASTNode* node, FILE* out)
{
    ASTNode* current = node;
    while (current)
    {
        switch (current->type)
        {
            case NODE_FUNCTION:
                fprintf(out, "A função %s ", current->value);
                if (current->left)
                {
                    fprintf(out, "recebe ");
                    ASTNode* param = current->left;
                    int count = 0;
                    ASTNode* tmp = param;
                    while (tmp)
                    {
                        count++;
                        tmp = tmp->next;
                    }

                    if (count == 1)
                        fprintf(out, "um valor ");
                    else if (count == 2)
                        fprintf(out, "dois valores ");
                    else
                        fprintf(out, "%d valores ", count);

                    fprintf(out, "%s, ", get_type_noun_pt(param->middle->value, count > 1));


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

                // If the first statement is a return, we can integrate the
                // "devolve [tipo]" better
                int has_return = 0;
                if (current->right && current->right->left)
                {
                    ASTNode* stmt = current->right->left;
                    if (stmt->type == NODE_RETURN && !stmt->next)
                    {
                        has_return = 1;
                    }
                }

                if (!has_return)
                {
                    fprintf(out, "devolve %s, ", current->middle->value);
                }

                // Process body
                if (current->right && current->right->left)
                {
                    ASTNode* stmt = current->right->left;
                    while (stmt)
                    {
                        translate_statement(stmt, out);
                        stmt = stmt->next;
                        if (stmt)
                            fprintf(out, ", ");
                        else
                            fprintf(out, ".");
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
                fprintf(out, "\n");
                break;

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
        current = current->next;
    }
}
