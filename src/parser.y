%{
// ============================================================================
// PARSER - Analisador Sintático (Bison)
// ============================================================================
// O parser recebe tokens do lexer e verifica se seguem as regras gramaticais.
// Constrói uma Árvore de Sintaxe Abstrata (AST) que representa a estrutura do programa.

#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

extern int yylex();           // Função do lexer
extern int yylineno;          // Número da linha atual
void yyerror(const char *s);  // Função de erro

ASTNode* root;  // Raiz da árvore de sintaxe abstrata
%}

// ============================================================================
// DECLARAÇÃO DE TIPOS DE VALORES
// ============================================================================
%union {
    char* sval;              // Valores string (identificadores, tipos)
    struct ASTNode* nval;    // Nós da árvore (expressões, statements)
}

// ============================================================================
// TOKENS - Símbolos terminais (vêm do lexer)
// ============================================================================
// Tokens que carregam valores (strings)
%token <sval> TYPE IDENTIFIER CONSTANT

// Tokens de palavras-chave de controle
%token IF ELSE SWITCH CASE DEFAULT FOR WHILE DO RETURN BREAK

// Tokens de operadores compostos
%token EQ NE LE GE AND OR SHL SHR
%token ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN AND_ASSIGN OR_ASSIGN XOR_ASSIGN SHL_ASSIGN SHR_ASSIGN

// ============================================================================
// NÃO-TERMINAIS - Regras gramaticais (retornam nós AST)
// ============================================================================
%type <nval> program external_declaration function_definition parameter_list parameter
%type <nval> block statement_list statement declaration assignment selection_statement iteration_statement jump_statement
%type <nval> expression assignment_expression conditional_expression logical_or_expression logical_and_expression
%type <nval> inclusive_or_expression exclusive_or_expression and_expression equality_expression relational_expression
%type <nval> shift_expression additive_expression multiplicative_expression cast_expression unary_expression postfix_expression primary_expression
%type <nval> argument_expression_list
%type <nval> labeled_statements labeled_statement constant_expression expression_opt

// ============================================================================
// RESOLUÇÃO DE PRECEDÊNCIA
// ============================================================================
// Resolve ambiguidade do if-else: if (x) if (y) z; else w;
// O else associa com o segundo if (devido à regra %nonassoc ELSE)
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

// ============================================================================
// REGRA INICIAL - PROGRAMA
// ============================================================================
program
    : external_declaration { root = $1; }  // Primeira declaração é a raiz
    | program external_declaration { $$ = append_node($1, $2); root = $$; }  // Conecta múltiplas declarações
    ;

// ============================================================================
// DECLARAÇÕES EXTERNAS (Top-level)
// ============================================================================
external_declaration
    : function_definition { $$ = $1; }  // Função
    | declaration { $$ = $1; }           // Variável global
    ;

// ============================================================================
// DEFINIÇÃO DE FUNÇÃO
// ============================================================================
// Exemplo: int add(int a, int b) { ... }
function_definition
    : TYPE IDENTIFIER '(' parameter_list ')' block {
        $$ = create_node(NODE_FUNCTION);
        $$->value = $2;                    // Nome da função
        $$->middle = create_node(NODE_IDENTIFIER);
        $$->middle->value = $1;            // Tipo de retorno
        $$->left = $4;                     // Parâmetros
        $$->right = $6;                    // Corpo (bloco de statements)
    }
    | TYPE IDENTIFIER '(' ')' block {     // Função sem parâmetros
        $$ = create_node(NODE_FUNCTION);
        $$->value = $2;
        $$->middle = create_node(NODE_IDENTIFIER);
        $$->middle->value = $1;
        $$->right = $5;
    }
    ;

// ============================================================================
// LISTA DE PARÂMETROS E PARÂMETROS
// ============================================================================
// Exemplo: (int a, float b, char c)
parameter_list
    : parameter { $$ = $1; }
    | parameter_list ',' parameter { $$ = append_node($1, $3); }  // Conecta múltiplos parâmetros
    ;

// Um parâmetro: tipo + nome
parameter
    : TYPE IDENTIFIER {
        $$ = create_node(NODE_PARAM);
        $$->value = $2;                // Nome do parâmetro
        $$->middle = create_node(NODE_IDENTIFIER);
        $$->middle->value = $1;        // Tipo do parâmetro
    }
    ;

// ============================================================================
// BLOCO - Agrupa múltiplos statements entre { }
// ============================================================================
block
    : '{' '}' { $$ = create_node(NODE_BLOCK); }  // Bloco vazio
    | '{' statement_list '}' { $$ = create_node(NODE_BLOCK); $$->left = $2; }  // Bloco com statements
    ;

// ============================================================================
// LISTA DE STATEMENTS - Múltiplos statements conectados
// ============================================================================
statement_list
    : statement { $$ = $1; }
    | statement_list statement { $$ = append_node($1, $2); }  // Conecta statements
    ;

// ============================================================================
// STATEMENT - Uma instrução ou comando
// ============================================================================
statement
    : declaration { $$ = $1; }           // int x;
    | assignment ';' { $$ = $1; }        // x = 5;
    | selection_statement { $$ = $1; }   // if (...) {...}
    | iteration_statement { $$ = $1; }   // while (...) {...}
    | jump_statement { $$ = $1; }        // return x;
    | expression ';' { $$ = $1; }        // func();
    | block { $$ = $1; }                 // { ... }
    ;

// ============================================================================
// DECLARAÇÃO - Define uma variável
// ============================================================================
declaration
    : TYPE IDENTIFIER ';' {
        // Exemplo: int x;
        $$ = create_node(NODE_DECLARATION);
        $$->value = $2;                    // Nome da variável
        $$->middle = create_node(NODE_IDENTIFIER);
        $$->middle->value = $1;            // Tipo da variável
    }
    | TYPE IDENTIFIER '=' assignment_expression ';' {
        // Exemplo: int x = 5;
        $$ = create_node(NODE_DECLARATION);
        $$->value = $2;                    // Nome da variável
        $$->middle = create_node(NODE_IDENTIFIER);
        $$->middle->value = $1;            // Tipo da variável
        $$->right = $4;                    // Valor inicial
    }
    ;

// ============================================================================
// ATRIBUIÇÃO - Atribui valor a uma variável
// ============================================================================
// Exemplos: x = 5, x += 3, x -= 2, etc.
assignment
    : primary_expression '=' assignment_expression { $$ = create_node(NODE_ASSIGNMENT); $$->left = $1; $$->right = $3; $$->op = strdup("="); }
    | primary_expression ADD_ASSIGN assignment_expression { $$ = create_node(NODE_ASSIGNMENT); $$->left = $1; $$->right = $3; $$->op = strdup("+="); }
    | primary_expression SUB_ASSIGN assignment_expression { $$ = create_node(NODE_ASSIGNMENT); $$->left = $1; $$->right = $3; $$->op = strdup("-="); }
    | primary_expression MUL_ASSIGN assignment_expression { $$ = create_node(NODE_ASSIGNMENT); $$->left = $1; $$->right = $3; $$->op = strdup("*="); }
    | primary_expression DIV_ASSIGN assignment_expression { $$ = create_node(NODE_ASSIGNMENT); $$->left = $1; $$->right = $3; $$->op = strdup("/="); }
    | primary_expression MOD_ASSIGN assignment_expression { $$ = create_node(NODE_ASSIGNMENT); $$->left = $1; $$->right = $3; $$->op = strdup("%="); }
    | primary_expression AND_ASSIGN assignment_expression { $$ = create_node(NODE_ASSIGNMENT); $$->left = $1; $$->right = $3; $$->op = strdup("&="); }
    | primary_expression OR_ASSIGN assignment_expression { $$ = create_node(NODE_ASSIGNMENT); $$->left = $1; $$->right = $3; $$->op = strdup("|="); }
    | primary_expression XOR_ASSIGN assignment_expression { $$ = create_node(NODE_ASSIGNMENT); $$->left = $1; $$->right = $3; $$->op = strdup("^="); }
    | primary_expression SHL_ASSIGN assignment_expression { $$ = create_node(NODE_ASSIGNMENT); $$->left = $1; $$->right = $3; $$->op = strdup("<<="); }
    | primary_expression SHR_ASSIGN assignment_expression { $$ = create_node(NODE_ASSIGNMENT); $$->left = $1; $$->right = $3; $$->op = strdup(">>="); }
    ;

// ============================================================================
// STATEMENT DE SELEÇÃO (if/switch) - Controla fluxo baseado em condição
// ============================================================================
selection_statement
    : IF '(' expression ')' statement %prec LOWER_THAN_ELSE {
        // Exemplo: if (x > 5) { ... }
        $$ = create_node(NODE_IF);
        $$->left = $3;       // Condição (expressão)
        $$->middle = $5;     // Corpo do if
    }
    | IF '(' expression ')' statement ELSE statement {
        // Exemplo: if (x > 5) { ... } else { ... }
        $$ = create_node(NODE_IF);
        $$->left = $3;       // Condição
        $$->middle = $5;     // Corpo do if
        $$->right = $7;      // Corpo do else
    }
    | SWITCH '(' expression ')' '{' labeled_statements '}' {
        // Exemplo: switch (x) { case 1: ... }
        $$ = create_node(NODE_SWITCH);
        $$->left = $3;       // Expressão a testar
        $$->right = $6;      // Lista de cases
    }
    ;

// ============================================================================
// LISTA DE CASES - Múltiplos cases dentro de switch
// ============================================================================
labeled_statements
    : labeled_statement { $$ = $1; }
    | labeled_statements labeled_statement { $$ = append_node($1, $2); }
    ;

// ============================================================================
// CASE - Uma opção dentro de switch
// ============================================================================
labeled_statement
    : CASE constant_expression ':' statement_list {
        // Exemplo: case 5: x = 10; y = 20;
        $$ = create_node(NODE_CASE);
        $$->left = $2;       // Valor do case
        $$->right = $4;      // Statements do case
    }
    | DEFAULT ':' statement_list {
        // Exemplo: default: x = 0;
        $$ = create_node(NODE_CASE);
        $$->right = $3;      // Statements do default
    }
    ;

constant_expression
    : conditional_expression { $$ = $1; }
    ;

// ============================================================================
// ITERATION STATEMENT (loops) - Repetição de código
// ============================================================================
iteration_statement
    : WHILE '(' expression ')' statement {
        // Exemplo: while (x < 10) { x++; }
        $$ = create_node(NODE_WHILE);
        $$->left = $3;       // Condição (testada antes de executar)
        $$->right = $5;      // Corpo do loop
    }
    | DO statement WHILE '(' expression ')' ';' {
        // Exemplo: do { x++; } while (x < 10);
        $$ = create_node(NODE_DO_WHILE);
        $$->left = $2;       // Corpo do loop
        $$->right = $5;      // Condição (testada após executar)
    }
    | FOR '(' expression_opt ';' expression_opt ';' expression_opt ')' statement {
        // Exemplo: for (i=0; i<10; i++) { ... }
        $$ = create_node(NODE_FOR);
        $$->left = $3;       // Inicialização (ex: i=0)
        $$->middle = $5;     // Condição (ex: i<10)
        $$->right = $9;      // Corpo do loop
        // Nota: incremento ($7) ignorado nesta versão simplificada
    }
    ;

// ============================================================================
// EXPRESSÃO OPCIONAL - Pode estar vazia
// ============================================================================
// Usada em for: for (;;) { } - todas as partes opcionais
expression_opt
    : /* empty */ { $$ = NULL; }      // Nada
    | expression { $$ = $1; }         // Expressão
    ;

// ============================================================================
// JUMP STATEMENT - Salta para outro ponto do código
// ============================================================================
jump_statement
    : RETURN expression ';' {
        // Exemplo: return x + 5;
        $$ = create_node(NODE_RETURN);
        $$->left = $2;       // Valor retornado
    }
    | RETURN ';' {
        // Exemplo: return;
        $$ = create_node(NODE_RETURN);
    }
    | BREAK ';' {
        // Exemplo: break;
        $$ = create_node(NODE_BREAK);  // Sai do loop/switch mais próximo
    }
    ;

// ============================================================================
// EXPRESSÕES - Descrevem valores e computações
// ============================================================================
// HIERARQUIA DE PRECEDÊNCIA (menor para maior):
// 1. expression (vírgula)
// 2. assignment_expression (=, +=, -=, etc)
// 3. conditional_expression (? :)
// 4. logical_or_expression (||)
// 5. logical_and_expression (&&)
// 6. inclusive_or_expression (|)
// 7. exclusive_or_expression (^)
// 8. and_expression (&)
// 9. equality_expression (==, !=)
// 10. relational_expression (<, >, <=, >=)
// 11. shift_expression (<<, >>)
// 12. additive_expression (+, -)
// 13. multiplicative_expression (*, /, %)
// 14. unary_expression (!, ~)
// 15. postfix_expression (chamada de função)
// 16. primary_expression (base: identificador, constante, parênteses)
// ============================================================================

expression
    : assignment_expression { $$ = $1; }
    | expression ',' assignment_expression { $$ = append_node($1, $3); }  // Múltiplas expressões
    ;

assignment_expression
    : conditional_expression { $$ = $1; }
    | assignment { $$ = $1; }  // Prioridade menor que condicional
    ;

// ============================================================================
// EXPRESSÃO CONDICIONAL (ternária)
// ============================================================================
// Exemplo: x > 5 ? 10 : 0
conditional_expression
    : logical_or_expression { $$ = $1; }
    | logical_or_expression '?' expression ':' conditional_expression {
        $$ = create_node(NODE_CONDITIONAL);
        $$->left = $1;       // Condição
        $$->middle = $3;     // Valor se verdadeiro
        $$->right = $5;      // Valor se falso
    }
    ;

// ============================================================================
// OPERADORES LÓGICOS
// ============================================================================
// Operador OU lógico (||) - menor precedência
logical_or_expression
    : logical_and_expression { $$ = $1; }
    | logical_or_expression OR logical_and_expression { $$ = create_binop("||", $1, $3); }
    ;

// Operador E lógico (&&) - maior precedência que ||
logical_and_expression
    : inclusive_or_expression { $$ = $1; }
    | logical_and_expression AND inclusive_or_expression { $$ = create_binop("&&", $1, $3); }
    ;

// ============================================================================
// OPERADORES BITWISE
// ============================================================================
// OU bit a bit (|)
inclusive_or_expression
    : exclusive_or_expression { $$ = $1; }
    | inclusive_or_expression '|' exclusive_or_expression { $$ = create_binop("|", $1, $3); }
    ;

// XOR bit a bit (^)
exclusive_or_expression
    : and_expression { $$ = $1; }
    | exclusive_or_expression '^' and_expression { $$ = create_binop("^", $1, $3); }
    ;

// E bit a bit (&) - maior precedência que | e ^
and_expression
    : equality_expression { $$ = $1; }
    | and_expression '&' equality_expression { $$ = create_binop("&", $1, $3); }
    ;

// ============================================================================
// OPERADORES DE COMPARAÇÃO
// ============================================================================
// Igualdade (==) e desigualdade (!=)
equality_expression
    : relational_expression { $$ = $1; }
    | equality_expression EQ relational_expression { $$ = create_binop("==", $1, $3); }
    | equality_expression NE relational_expression { $$ = create_binop("!=", $1, $3); }
    ;

// Relacional (<, >, <=, >=)
relational_expression
    : shift_expression { $$ = $1; }
    | relational_expression '<' shift_expression { $$ = create_binop("<", $1, $3); }
    | relational_expression '>' shift_expression { $$ = create_binop(">", $1, $3); }
    | relational_expression LE shift_expression { $$ = create_binop("<=", $1, $3); }
    | relational_expression GE shift_expression { $$ = create_binop(">=", $1, $3); }
    ;

// ============================================================================
// DESLOCAMENTO DE BITS
// ============================================================================
// << (esquerda) e >> (direita)
shift_expression
    : additive_expression { $$ = $1; }
    | shift_expression SHL additive_expression { $$ = create_binop("<<", $1, $3); }
    | shift_expression SHR additive_expression { $$ = create_binop(">>", $1, $3); }
    ;

// ============================================================================
// ARITMÉTICA
// ============================================================================
// Adição (+) e subtração (-)
additive_expression
    : multiplicative_expression { $$ = $1; }
    | additive_expression '+' multiplicative_expression { $$ = create_binop("+", $1, $3); }
    | additive_expression '-' multiplicative_expression { $$ = create_binop("-", $1, $3); }
    ;

// Multiplicação (*), divisão (/) e módulo (%)
multiplicative_expression
    : cast_expression { $$ = $1; }
    | multiplicative_expression '*' cast_expression { $$ = create_binop("*", $1, $3); }
    | multiplicative_expression '/' cast_expression { $$ = create_binop("/", $1, $3); }
    | multiplicative_expression '%' cast_expression { $$ = create_binop("%", $1, $3); }
    ;

cast_expression
    : unary_expression { $$ = $1; }
    ;

// ============================================================================
// OPERADORES UNÁRIOS (atuam sobre um único operando)
// ============================================================================
unary_expression
    : postfix_expression { $$ = $1; }
    | '!' unary_expression {
        // Negação lógica (NOT)
        $$ = create_node(NODE_UNOP);
        $$->op = strdup("!");
        $$->left = $2;
    }
    | '~' unary_expression {
        // Complemento bit a bit (NOT bitwise)
        $$ = create_node(NODE_UNOP);
        $$->op = strdup("~");
        $$->left = $2;
    }
    ;

// ============================================================================
// EXPRESSÃO PÓS-FIX (funções e bases)
// ============================================================================
postfix_expression
    : primary_expression { $$ = $1; }
    | IDENTIFIER '(' ')' {
        // Chamada de função sem argumentos
        $$ = create_node(NODE_CALL);
        $$->value = $1;      // Nome da função
    }
    | IDENTIFIER '(' argument_expression_list ')' {
        // Chamada de função com argumentos
        $$ = create_node(NODE_CALL);
        $$->value = $1;      // Nome da função
        $$->left = $3;       // Lista de argumentos
    }
    ;

// ============================================================================
// EXPRESSÃO PRIMÁRIA (base da hierarquia)
// ============================================================================
primary_expression
    : IDENTIFIER { $$ = create_id($1); }         // Nome de variável/função
    | CONSTANT { $$ = create_const($1); }        // Literal (número)
    | '(' expression ')' { $$ = $2; }            // Expressão entre parênteses
    ;

// ============================================================================
// LISTA DE ARGUMENTOS (parâmetros em chamada de função)
// ============================================================================
argument_expression_list
    : assignment_expression { $$ = $1; }
    | argument_expression_list ',' assignment_expression { $$ = append_node($1, $3); }
    ;

%%

// ============================================================================
// TRATAMENTO DE ERROS
// ============================================================================
// Função chamada pelo Bison quando encontra um erro sintático
void yyerror(const char *s) {
    fprintf(stderr, "Erro sintático na linha %d: %s\n", yylineno, s);
}
