// ============================================================================
// TRANSLATOR - Translates AST to Portuguese natural language
// ============================================================================
// Converts the abstract syntax tree into Portuguese natural language
// descriptions. Example: x + 5 → "a soma de x e 5"

#include "translator.h"
#include <stdio.h>
#include <string.h>

// Forward declaration (used recursively)
void translate_expression(ASTNode* node, FILE* out);

// ============================================================================
// get_type_pt - Map a type name to its Portuguese adjective form
// ============================================================================
// Converts technical type names to Portuguese descriptions.
// Examples:
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
// get_type_noun_pt - Return the Portuguese noun form of a type
// (singular/plural)
// ============================================================================
// Adapts gender and number.
// Examples:
//   ("int", 0) → "inteiro"
//   ("int", 1) → "inteiros"
//   ("float", 0) → "real"
const char* get_type_noun_pt(char* type, int plural)
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
// translate_call - Translate a function call
// ============================================================================
// Examples:
//   print(5) → "o resultado da chamada à função print, que recebe o valor 5"
//   add(a, b) → "o resultado da chamada à função add, que recebe os valores a e
//   b"
void translate_call(ASTNode* node, FILE* out)
{
    fprintf(out, "o resultado da chamada à função %s", node->value);

    if (node->left) // if there are arguments
    {
        fprintf(out, ", que recebe ");

        // count the number of arguments
        ASTNode* arg = node->left;
        int count = 0;
        ASTNode* tmp = arg;
        while (tmp)
        {
            count++;
            tmp = tmp->next;
        }

        // choose "valor" or "valores" (singular/plural)
        if (count == 1)
            fprintf(out, "o valor ");
        else
            fprintf(out, "os valores ");

        // translate each argument, separating with comma or "e"
        while (arg)
        {
            translate_expression(arg, out);
            arg = arg->next;
            if (arg)
            {
                if (arg->next)
                    fprintf(out, ", "); // comma if more arguments follow
                else
                    fprintf(out, " e "); // "e" before the last argument
            }
        }
    }
}

// ============================================================================
// translate_expression - Translate expressions to Portuguese
// ============================================================================
// Processes expression nodes and writes the Portuguese translation.
// Examples:
//   x + 5 → "a soma de x e 5"
//   x > 10 → "x é maior que 10"
//   !x → "a negação lógica de x"
void translate_expression(ASTNode* node, FILE* out)
{
    if (!node)
        return;

    // process each expression node type
    switch (node->type)
    {
        case NODE_IDENTIFIER:
            // variable or function name: write it directly
            fprintf(out, "%s", node->value);
            break;

        case NODE_CONSTANT:
            // literal value: write it directly
            fprintf(out, "%s", node->value);
            break;

        case NODE_CALL:
            // function call: delegate to translate_call
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
            // binary operator (two operands)
            // ============ ARITHMETIC OPERATIONS ============
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
            // ============ COMPARISON OPERATIONS ============
            else if (strcmp(node->op, "==") == 0)
            {
                // equality
                translate_expression(node->left, out);
                fprintf(out, " é igual a ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "!=") == 0)
            {
                // inequality
                translate_expression(node->left, out);
                fprintf(out, " é diferente de ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, ">") == 0)
            {
                // greater than
                translate_expression(node->left, out);
                fprintf(out, " é maior que ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "<") == 0)
            {
                // less than
                translate_expression(node->left, out);
                fprintf(out, " é menor que ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, ">=") == 0)
            {
                // greater than or equal
                translate_expression(node->left, out);
                fprintf(out, " é maior ou igual a ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "<=") == 0)
            {
                // less than or equal
                translate_expression(node->left, out);
                fprintf(out, " é menor ou igual a ");
                translate_expression(node->right, out);
            }
            // ============ BITWISE AND SHIFT OPERATIONS ============
            else if (strcmp(node->op, "<<") == 0)
            {
                // left shift
                fprintf(out, "o deslocamento de ");
                translate_expression(node->left, out);
                fprintf(out, " à esquerda por ");
                translate_expression(node->right, out);
                fprintf(out, " bits");
            }
            else if (strcmp(node->op, ">>") == 0)
            {
                // right shift
                fprintf(out, "o deslocamento de ");
                translate_expression(node->left, out);
                fprintf(out, " à direita por ");
                translate_expression(node->right, out);
                fprintf(out, " bits");
            }
            else if (strcmp(node->op, "&") == 0)
            {
                // bitwise AND
                fprintf(out, "o AND bit a bit entre ");
                translate_expression(node->left, out);
                fprintf(out, " e ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "|") == 0)
            {
                // bitwise OR
                fprintf(out, "o OR bit a bit entre ");
                translate_expression(node->left, out);
                fprintf(out, " e ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "^") == 0)
            {
                // bitwise XOR
                fprintf(out, "o XOR bit a bit entre ");
                translate_expression(node->left, out);
                fprintf(out, " e ");
                translate_expression(node->right, out);
            }
            // ============ LOGICAL OPERATORS ============
            else if (strcmp(node->op, "&&") == 0)
            {
                // logical AND
                translate_expression(node->left, out);
                fprintf(out, " e ");
                translate_expression(node->right, out);
            }
            else if (strcmp(node->op, "||") == 0)
            {
                // logical OR
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
// translate_statement - Translate statements
// ============================================================================
// Processes declarations, assignments, and control flow statements.
void translate_statement(ASTNode* node, FILE* out)
{
    if (!node)
        return;

    switch (node->type)
    {
        // ============ DECLARATION ============
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

        // ============ ASSIGNMENT ============
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

        // ============ RETURN ============
        case NODE_RETURN:
            fprintf(out, "devolve ");
            translate_expression(node->left, out);
            break;

        // ============ CONDITIONAL (IF/ELSE) ============
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

        // ============ WHILE LOOP ============
        case NODE_WHILE:
            fprintf(out, "enquanto ");
            translate_expression(node->left, out);
            fprintf(out, " for verdade, executa { ");
            translate_ast(node->right, out);
            fprintf(out, " }");
            break;

        // ============ DO-WHILE LOOP ============
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

        // ============ FOR LOOP ============
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

        // ============ CASE (inside SWITCH) ============
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

        // ============ BLOCK ============
        case NODE_BLOCK:
            // translate statements inside the block
            translate_ast(node->left, out);
            break;

        // ============ FUNCTION CALL (as statement) ============
        case NODE_CALL:
            fprintf(out, "chama a função %s", node->value);
            break;

        default:
            break;
    }
}

// ============================================================================
// translate_ast - Translate the entire AST
// ============================================================================
// Entry point: processes each AST node from root to leaves.
// Traverses the linked list of nodes (via next) to the end.
void translate_ast(ASTNode* node, FILE* out)
{
    ASTNode* current = node;

    // process each node in the linked list
    while (current)
    {
        switch (current->type)
        {
            // ============ FUNCTION ============
            case NODE_FUNCTION:
                fprintf(out, "A função %s ", current->value);

                // translate parameters (if any)
                if (current->left)
                {
                    fprintf(out, "recebe ");
                    ASTNode* param = current->left;

                    // count parameters
                    int count = 0;
                    ASTNode* tmp = param;
                    while (tmp)
                    {
                        count++;
                        tmp = tmp->next;
                    }

                    // adapt for singular/plural
                    if (count == 1)
                        fprintf(out, "um valor ");
                    else if (count == 2)
                        fprintf(out, "dois valores ");
                    else
                        fprintf(out, "%d valores ", count);

                    // parameter type
                    fprintf(
                        out,
                        "%s, ",
                        get_type_noun_pt(param->middle->value, count > 1)
                    );

                    // list parameter names
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

                // check whether the body is a single return (shortcut
                // translation)
                int has_return = 0;
                if (current->right && current->right->left)
                {
                    ASTNode* stmt = current->right->left;
                    if (stmt->type == NODE_RETURN && !stmt->next)
                    {
                        has_return = 1;
                    }
                }

                // translate return type (only when body is not just a return)
                if (!has_return)
                {
                    fprintf(out, "devolve %s, ", current->middle->value);
                }

                // translate function body (statements)
                if (current->right && current->right->left)
                {
                    ASTNode* stmt = current->right->left;
                    while (stmt)
                    {
                        translate_statement(stmt, out);
                        stmt = stmt->next;
                        if (stmt)
                            fprintf(out, ", "); // comma between statements
                        else
                            fprintf(out, "."); // trailing period
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
                fprintf(out, "\n"); // newline after each function
                break;

            // ============ OTHER TOP-LEVEL STATEMENTS ============
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

        // advance to next node
        current = current->next;
    }
}
