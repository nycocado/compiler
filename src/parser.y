%{
// ============================================================================
// PARSER - Syntactic Analyser (Bison)
// ============================================================================
// Receives tokens from the lexer and verifies they follow the grammar rules.
// Builds an Abstract Syntax Tree (AST) representing the program's structure.

#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

extern int yylex();           // lexer function
extern int yylineno;          // current line number
void yyerror(const char *s);  // error function

ASTNode* root;  // AST root
%}

// ============================================================================
// VALUE TYPE DECLARATIONS
// ============================================================================
%union {
    char* sval;              // string values (identifiers, types)
    struct ASTNode* nval;    // AST nodes (expressions, statements)
}

// ============================================================================
// TOKENS - Terminal symbols (from the lexer)
// ============================================================================
// Tokens that carry values (strings)
%token <sval> TYPE IDENTIFIER CONSTANT

// Control keyword tokens
%token IF ELSE SWITCH CASE DEFAULT FOR WHILE DO RETURN BREAK

// Compound operator tokens
%token EQ NE LE GE AND OR SHL SHR
%token ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN AND_ASSIGN OR_ASSIGN XOR_ASSIGN SHL_ASSIGN SHR_ASSIGN

// ============================================================================
// NON-TERMINALS - Grammar rules (return AST nodes)
// ============================================================================
%type <nval> program external_declaration function_definition parameter_list parameter
%type <nval> block statement_list statement declaration declarator_list declarator assignment selection_statement iteration_statement jump_statement
%type <nval> expression assignment_expression conditional_expression logical_or_expression logical_and_expression
%type <nval> inclusive_or_expression exclusive_or_expression and_expression equality_expression relational_expression
%type <nval> shift_expression additive_expression multiplicative_expression cast_expression unary_expression postfix_expression primary_expression
%type <nval> argument_expression_list
%type <nval> labeled_statements labeled_statement constant_expression expression_opt

// ============================================================================
// PRECEDENCE RESOLUTION
// ============================================================================
// Resolves the dangling-else ambiguity: if (x) if (y) z; else w;
// The else binds to the innermost if (%nonassoc ELSE)
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

// ============================================================================
// START RULE - PROGRAM
// ============================================================================
program
    : external_declaration { root = $1; }  // first declaration is the root
    | program external_declaration { $$ = append_node($1, $2); root = $$; }  // connect multiple declarations
    ;

// ============================================================================
// EXTERNAL DECLARATIONS (top-level)
// ============================================================================
external_declaration
    : function_definition { $$ = $1; }  // function
    | declaration { $$ = $1; }           // global variable
    ;

// ============================================================================
// FUNCTION DEFINITION
// ============================================================================
// Example: int add(int a, int b) { ... }
function_definition
    : TYPE IDENTIFIER '(' parameter_list ')' block {
        $$ = create_node(NODE_FUNCTION);
        $$->value = $2;                    // function name
        $$->middle = create_node(NODE_IDENTIFIER);
        $$->middle->value = $1;            // return type
        $$->left = $4;                     // parameters
        $$->right = $6;                    // body (statement block)
    }
    | TYPE IDENTIFIER '(' ')' block {     // function with no parameters
        $$ = create_node(NODE_FUNCTION);
        $$->value = $2;
        $$->middle = create_node(NODE_IDENTIFIER);
        $$->middle->value = $1;
        $$->right = $5;
    }
    ;

// ============================================================================
// PARAMETER LIST AND PARAMETERS
// ============================================================================
// Example: (int a, float b, char c)
parameter_list
    : parameter { $$ = $1; }
    | parameter_list ',' parameter { $$ = append_node($1, $3); }  // connect multiple parameters
    ;

// a single parameter: type + name
parameter
    : TYPE IDENTIFIER {
        $$ = create_node(NODE_PARAM);
        $$->value = $2;                // parameter name
        $$->middle = create_node(NODE_IDENTIFIER);
        $$->middle->value = $1;        // parameter type
    }
    ;

// ============================================================================
// BLOCK - Groups multiple statements between { }
// ============================================================================
block
    : '{' '}' { $$ = create_node(NODE_BLOCK); }  // empty block
    | '{' statement_list '}' { $$ = create_node(NODE_BLOCK); $$->left = $2; }  // block with statements
    ;

// ============================================================================
// STATEMENT LIST - Multiple connected statements
// ============================================================================
statement_list
    : statement { $$ = $1; }
    | statement_list statement { $$ = append_node($1, $2); }  // connect statements
    ;

// ============================================================================
// STATEMENT - A single instruction or command
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
// DECLARATION - Defines one or more variables of the same type
// ============================================================================
// Examples: int x;   float a, b, c;   int x=0, y=1;
declaration
    : TYPE declarator_list ';' { $$ = set_decl_type($2, $1); }
    ;

declarator_list
    : declarator { $$ = $1; }
    | declarator_list ',' declarator { $$ = append_node($1, $3); }
    ;

declarator
    : IDENTIFIER {
        $$ = create_node(NODE_DECLARATION);
        $$->value = $1;
    }
    | IDENTIFIER '=' assignment_expression {
        $$ = create_node(NODE_DECLARATION);
        $$->value = $1;
        $$->right = $3;
    }
    ;

// ============================================================================
// ASSIGNMENT - Assigns a value to a variable
// ============================================================================
// Examples: x = 5, x += 3, x -= 2, etc.
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
// SELECTION STATEMENT (if/switch) - Controls flow based on a condition
// ============================================================================
selection_statement
    : IF '(' expression ')' statement %prec LOWER_THAN_ELSE {
        // Example: if (x > 5) { ... }
        $$ = create_node(NODE_IF);
        $$->left = $3;       // condition (expression)
        $$->middle = $5;     // if body
    }
    | IF '(' expression ')' statement ELSE statement {
        // Example: if (x > 5) { ... } else { ... }
        $$ = create_node(NODE_IF);
        $$->left = $3;       // condition
        $$->middle = $5;     // if body
        $$->right = $7;      // else body
    }
    | SWITCH '(' expression ')' '{' labeled_statements '}' {
        // Example: switch (x) { case 1: ... }
        $$ = create_node(NODE_SWITCH);
        $$->left = $3;       // expression to test
        $$->right = $6;      // case list
    }
    ;

// ============================================================================
// CASE LIST - Multiple cases inside a switch
// ============================================================================
labeled_statements
    : labeled_statement { $$ = $1; }
    | labeled_statements labeled_statement { $$ = append_node($1, $2); }
    ;

// ============================================================================
// CASE - A single option inside a switch
// ============================================================================
labeled_statement
    : CASE constant_expression ':' statement_list {
        // Example: case 5: x = 10; y = 20;
        $$ = create_node(NODE_CASE);
        $$->left = $2;       // case value
        $$->right = $4;      // case statements
    }
    | DEFAULT ':' statement_list {
        // Example: default: x = 0;
        $$ = create_node(NODE_CASE);
        $$->right = $3;      // default statements
    }
    ;

constant_expression
    : conditional_expression { $$ = $1; }
    ;

// ============================================================================
// ITERATION STATEMENT (loops) - Code repetition
// ============================================================================
iteration_statement
    : WHILE '(' expression ')' statement {
        // Example: while (x < 10) { x++; }
        $$ = create_node(NODE_WHILE);
        $$->left = $3;       // condition (tested before executing)
        $$->right = $5;      // loop body
    }
    | DO statement WHILE '(' expression ')' ';' {
        // Example: do { x++; } while (x < 10);
        $$ = create_node(NODE_DO_WHILE);
        $$->left = $2;       // loop body
        $$->right = $5;      // condition (tested after executing)
    }
    | FOR '(' expression_opt ';' expression_opt ';' expression_opt ')' statement {
        // Example: for (i=0; i<10; i++) { ... }
        $$ = create_node(NODE_FOR);
        $$->left = $3;       // initializer (e.g. i=0)
        $$->middle = $5;     // condition (e.g. i<10)
        $$->right = $9;      // loop body
        // Note: increment ($7) ignored in this simplified version
    }
    ;

// ============================================================================
// OPTIONAL EXPRESSION - May be empty
// ============================================================================
// Used in for: for (;;) { } - all parts optional
expression_opt
    : /* empty */ { $$ = NULL; }      // nothing
    | expression { $$ = $1; }         // expression
    ;

// ============================================================================
// JUMP STATEMENT - Jumps to another point in the code
// ============================================================================
jump_statement
    : RETURN expression ';' {
        // Example: return x + 5;
        $$ = create_node(NODE_RETURN);
        $$->left = $2;       // returned value
    }
    | RETURN ';' {
        // Example: return;
        $$ = create_node(NODE_RETURN);
    }
    | BREAK ';' {
        // Example: break;
        $$ = create_node(NODE_BREAK);  // exits the nearest loop/switch
    }
    ;

// ============================================================================
// EXPRESSIONS - Describe values and computations
// ============================================================================
// PRECEDENCE HIERARCHY (lowest to highest):
// 1. expression (comma)
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
// 15. postfix_expression (function call)
// 16. primary_expression (base: identifier, constant, parentheses)
// ============================================================================

expression
    : assignment_expression { $$ = $1; }
    | expression ',' assignment_expression { $$ = append_node($1, $3); }  // multiple expressions
    ;

assignment_expression
    : conditional_expression { $$ = $1; }
    | assignment { $$ = $1; }  // lower priority than conditional
    ;

// ============================================================================
// CONDITIONAL EXPRESSION (ternary)
// ============================================================================
// Example: x > 5 ? 10 : 0
conditional_expression
    : logical_or_expression { $$ = $1; }
    | logical_or_expression '?' expression ':' conditional_expression {
        $$ = create_node(NODE_CONDITIONAL);
        $$->left = $1;       // condition
        $$->middle = $3;     // value if true
        $$->right = $5;      // value if false
    }
    ;

// ============================================================================
// LOGICAL OPERATORS
// ============================================================================
// logical OR (||) - lowest precedence
logical_or_expression
    : logical_and_expression { $$ = $1; }
    | logical_or_expression OR logical_and_expression { $$ = create_binop("||", $1, $3); }
    ;

// logical AND (&&) - higher precedence than ||
logical_and_expression
    : inclusive_or_expression { $$ = $1; }
    | logical_and_expression AND inclusive_or_expression { $$ = create_binop("&&", $1, $3); }
    ;

// ============================================================================
// BITWISE OPERATORS
// ============================================================================
// bitwise OR (|)
inclusive_or_expression
    : exclusive_or_expression { $$ = $1; }
    | inclusive_or_expression '|' exclusive_or_expression { $$ = create_binop("|", $1, $3); }
    ;

// bitwise XOR (^)
exclusive_or_expression
    : and_expression { $$ = $1; }
    | exclusive_or_expression '^' and_expression { $$ = create_binop("^", $1, $3); }
    ;

// bitwise AND (&) - higher precedence than | and ^
and_expression
    : equality_expression { $$ = $1; }
    | and_expression '&' equality_expression { $$ = create_binop("&", $1, $3); }
    ;

// ============================================================================
// COMPARISON OPERATORS
// ============================================================================
// equality (==) and inequality (!=)
equality_expression
    : relational_expression { $$ = $1; }
    | equality_expression EQ relational_expression { $$ = create_binop("==", $1, $3); }
    | equality_expression NE relational_expression { $$ = create_binop("!=", $1, $3); }
    ;

// relational (<, >, <=, >=)
relational_expression
    : shift_expression { $$ = $1; }
    | relational_expression '<' shift_expression { $$ = create_binop("<", $1, $3); }
    | relational_expression '>' shift_expression { $$ = create_binop(">", $1, $3); }
    | relational_expression LE shift_expression { $$ = create_binop("<=", $1, $3); }
    | relational_expression GE shift_expression { $$ = create_binop(">=", $1, $3); }
    ;

// ============================================================================
// BIT SHIFT
// ============================================================================
// << (left) and >> (right)
shift_expression
    : additive_expression { $$ = $1; }
    | shift_expression SHL additive_expression { $$ = create_binop("<<", $1, $3); }
    | shift_expression SHR additive_expression { $$ = create_binop(">>", $1, $3); }
    ;

// ============================================================================
// ARITHMETIC
// ============================================================================
// addition (+) and subtraction (-)
additive_expression
    : multiplicative_expression { $$ = $1; }
    | additive_expression '+' multiplicative_expression { $$ = create_binop("+", $1, $3); }
    | additive_expression '-' multiplicative_expression { $$ = create_binop("-", $1, $3); }
    ;

// multiplication (*), division (/) and modulo (%)
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
// UNARY OPERATORS (act on a single operand)
// ============================================================================
unary_expression
    : postfix_expression { $$ = $1; }
    | '-' unary_expression {
        $$ = create_node(NODE_UNOP);
        $$->op = strdup("-");
        $$->left = $2;
    }
    | '+' unary_expression {
        $$ = create_node(NODE_UNOP);
        $$->op = strdup("+");
        $$->left = $2;
    }
    | '!' unary_expression {
        $$ = create_node(NODE_UNOP);
        $$->op = strdup("!");
        $$->left = $2;
    }
    | '~' unary_expression {
        $$ = create_node(NODE_UNOP);
        $$->op = strdup("~");
        $$->left = $2;
    }
    ;

// ============================================================================
// POSTFIX EXPRESSION (functions and bases)
// ============================================================================
postfix_expression
    : primary_expression { $$ = $1; }
    | IDENTIFIER '(' ')' {
        // function call with no arguments
        $$ = create_node(NODE_CALL);
        $$->value = $1;      // function name
    }
    | IDENTIFIER '(' argument_expression_list ')' {
        // function call with arguments
        $$ = create_node(NODE_CALL);
        $$->value = $1;      // function name
        $$->left = $3;       // argument list
    }
    ;

// ============================================================================
// PRIMARY EXPRESSION (base of the hierarchy)
// ============================================================================
primary_expression
    : IDENTIFIER { $$ = create_id($1); }         // variable/function name
    | CONSTANT { $$ = create_const($1); }        // literal (number)
    | '(' expression ')' { $$ = $2; }            // expression in parentheses
    ;

// ============================================================================
// ARGUMENT LIST (parameters in a function call)
// ============================================================================
argument_expression_list
    : assignment_expression { $$ = $1; }
    | argument_expression_list ',' assignment_expression { $$ = append_node($1, $3); }
    ;

%%

// ============================================================================
// ERROR HANDLING
// ============================================================================
// Called by Bison when a syntax error is encountered
void yyerror(const char *s) {
    fprintf(stderr, "Erro sintático na linha %d: %s\n", yylineno, s);
}
