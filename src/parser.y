%{
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

extern int yylex();
extern int yylineno;
void yyerror(const char *s);

ASTNode* root;
%}

%union {
    char* sval;
    struct ASTNode* nval;
}

%token <sval> TYPE IDENTIFIER CONSTANT
%token IF ELSE SWITCH CASE DEFAULT FOR WHILE DO RETURN BREAK
%token EQ NE LE GE AND OR SHL SHR
%token ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN AND_ASSIGN OR_ASSIGN XOR_ASSIGN SHL_ASSIGN SHR_ASSIGN

%type <nval> program external_declaration function_definition parameter_list parameter
%type <nval> block statement_list statement declaration assignment selection_statement iteration_statement jump_statement
%type <nval> expression assignment_expression conditional_expression logical_or_expression logical_and_expression
%type <nval> inclusive_or_expression exclusive_or_expression and_expression equality_expression relational_expression
%type <nval> shift_expression additive_expression multiplicative_expression cast_expression unary_expression postfix_expression primary_expression
%type <nval> argument_expression_list
%type <nval> labeled_statements labeled_statement constant_expression expression_opt

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

program
    : external_declaration { root = $1; }
    | program external_declaration { $$ = append_node($1, $2); root = $$; }
    ;

external_declaration
    : function_definition { $$ = $1; }
    | declaration { $$ = $1; }
    ;

function_definition
    : TYPE IDENTIFIER '(' parameter_list ')' block {
        $$ = create_node(NODE_FUNCTION);
        $$->value = $2;
        $$->middle = create_node(NODE_IDENTIFIER);
        $$->middle->value = $1;
        $$->left = $4;
        $$->right = $6;
    }
    | TYPE IDENTIFIER '(' ')' block {
        $$ = create_node(NODE_FUNCTION);
        $$->value = $2;
        $$->middle = create_node(NODE_IDENTIFIER);
        $$->middle->value = $1;
        $$->right = $5;
    }
    ;

parameter_list
    : parameter { $$ = $1; }
    | parameter_list ',' parameter { $$ = append_node($1, $3); }
    ;

parameter
    : TYPE IDENTIFIER {
        $$ = create_node(NODE_PARAM);
        $$->value = $2;
        $$->middle = create_node(NODE_IDENTIFIER);
        $$->middle->value = $1;
    }
    ;

block
    : '{' '}' { $$ = create_node(NODE_BLOCK); }
    | '{' statement_list '}' { $$ = create_node(NODE_BLOCK); $$->left = $2; }
    ;

statement_list
    : statement { $$ = $1; }
    | statement_list statement { $$ = append_node($1, $2); }
    ;

statement
    : declaration { $$ = $1; }
    | assignment ';' { $$ = $1; }
    | selection_statement { $$ = $1; }
    | iteration_statement { $$ = $1; }
    | jump_statement { $$ = $1; }
    | expression ';' { $$ = $1; }
    | block { $$ = $1; }
    ;

declaration
    : TYPE IDENTIFIER ';' {
        $$ = create_node(NODE_DECLARATION);
        $$->value = $2;
        $$->middle = create_node(NODE_IDENTIFIER);
        $$->middle->value = $1;
    }
    | TYPE IDENTIFIER '=' assignment_expression ';' {
        $$ = create_node(NODE_DECLARATION);
        $$->value = $2;
        $$->middle = create_node(NODE_IDENTIFIER);
        $$->middle->value = $1;
        $$->right = $4;
    }
    ;

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

selection_statement
    : IF '(' expression ')' statement %prec LOWER_THAN_ELSE {
        $$ = create_node(NODE_IF);
        $$->left = $3;
        $$->middle = $5;
    }
    | IF '(' expression ')' statement ELSE statement {
        $$ = create_node(NODE_IF);
        $$->left = $3;
        $$->middle = $5;
        $$->right = $7;
    }
    | SWITCH '(' expression ')' '{' labeled_statements '}' {
        $$ = create_node(NODE_SWITCH);
        $$->left = $3;
        $$->right = $6;
    }
    ;

labeled_statements
    : labeled_statement { $$ = $1; }
    | labeled_statements labeled_statement { $$ = append_node($1, $2); }
    ;

labeled_statement
    : CASE constant_expression ':' statement_list {
        $$ = create_node(NODE_CASE);
        $$->left = $2;
        $$->right = $4;
    }
    | DEFAULT ':' statement_list {
        $$ = create_node(NODE_CASE);
        $$->right = $3;
    }
    ;

constant_expression
    : conditional_expression { $$ = $1; }
    ;

iteration_statement
    : WHILE '(' expression ')' statement {
        $$ = create_node(NODE_WHILE);
        $$->left = $3;
        $$->right = $5;
    }
    | DO statement WHILE '(' expression ')' ';' {
        $$ = create_node(NODE_DO_WHILE);
        $$->left = $2;
        $$->right = $5;
    }
    | FOR '(' expression_opt ';' expression_opt ';' expression_opt ')' statement {
        $$ = create_node(NODE_FOR);
        $$->left = $3;   // init
        $$->middle = $5; // cond
        $$->right = $9;  // body
        // Note: simplified, ignoring 'next' part of for for Portuguese description
    }
    ;

expression_opt
    : /* empty */ { $$ = NULL; }
    | expression { $$ = $1; }
    ;

jump_statement
    : RETURN expression ';' {
        $$ = create_node(NODE_RETURN);
        $$->left = $2;
    }
    | RETURN ';' {
        $$ = create_node(NODE_RETURN);
    }
    | BREAK ';' {
        $$ = create_node(NODE_BREAK);
    }
    ;

expression
    : assignment_expression { $$ = $1; }
    | expression ',' assignment_expression { $$ = append_node($1, $3); }
    ;

assignment_expression
    : conditional_expression { $$ = $1; }
    | assignment { $$ = $1; }
    ;

conditional_expression
    : logical_or_expression { $$ = $1; }
    | logical_or_expression '?' expression ':' conditional_expression {
        $$ = create_node(NODE_CONDITIONAL);
        $$->left = $1;
        $$->middle = $3;
        $$->right = $5;
    }
    ;

logical_or_expression
    : logical_and_expression { $$ = $1; }
    | logical_or_expression OR logical_and_expression { $$ = create_binop("||", $1, $3); }
    ;

logical_and_expression
    : inclusive_or_expression { $$ = $1; }
    | logical_and_expression AND inclusive_or_expression { $$ = create_binop("&&", $1, $3); }
    ;

inclusive_or_expression
    : exclusive_or_expression { $$ = $1; }
    | inclusive_or_expression '|' exclusive_or_expression { $$ = create_binop("|", $1, $3); }
    ;

exclusive_or_expression
    : and_expression { $$ = $1; }
    | exclusive_or_expression '^' and_expression { $$ = create_binop("^", $1, $3); }
    ;

and_expression
    : equality_expression { $$ = $1; }
    | and_expression '&' equality_expression { $$ = create_binop("&", $1, $3); }
    ;

equality_expression
    : relational_expression { $$ = $1; }
    | equality_expression EQ relational_expression { $$ = create_binop("==", $1, $3); }
    | equality_expression NE relational_expression { $$ = create_binop("!=", $1, $3); }
    ;

relational_expression
    : shift_expression { $$ = $1; }
    | relational_expression '<' shift_expression { $$ = create_binop("<", $1, $3); }
    | relational_expression '>' shift_expression { $$ = create_binop(">", $1, $3); }
    | relational_expression LE shift_expression { $$ = create_binop("<=", $1, $3); }
    | relational_expression GE shift_expression { $$ = create_binop(">=", $1, $3); }
    ;

shift_expression
    : additive_expression { $$ = $1; }
    | shift_expression SHL additive_expression { $$ = create_binop("<<", $1, $3); }
    | shift_expression SHR additive_expression { $$ = create_binop(">>", $1, $3); }
    ;

additive_expression
    : multiplicative_expression { $$ = $1; }
    | additive_expression '+' multiplicative_expression { $$ = create_binop("+", $1, $3); }
    | additive_expression '-' multiplicative_expression { $$ = create_binop("-", $1, $3); }
    ;

multiplicative_expression
    : cast_expression { $$ = $1; }
    | multiplicative_expression '*' cast_expression { $$ = create_binop("*", $1, $3); }
    | multiplicative_expression '/' cast_expression { $$ = create_binop("/", $1, $3); }
    | multiplicative_expression '%' cast_expression { $$ = create_binop("%", $1, $3); }
    ;

cast_expression
    : unary_expression { $$ = $1; }
    ;

unary_expression
    : postfix_expression { $$ = $1; }
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

postfix_expression
    : primary_expression { $$ = $1; }
    | IDENTIFIER '(' ')' {
        $$ = create_node(NODE_CALL);
        $$->value = $1;
    }
    | IDENTIFIER '(' argument_expression_list ')' {
        $$ = create_node(NODE_CALL);
        $$->value = $1;
        $$->left = $3;
    }
    ;

primary_expression
    : IDENTIFIER { $$ = create_id($1); }
    | CONSTANT { $$ = create_const($1); }
    | '(' expression ')' { $$ = $2; }
    ;

argument_expression_list
    : assignment_expression { $$ = $1; }
    | argument_expression_list ',' assignment_expression { $$ = append_node($1, $3); }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintático na linha %d: %s\n", yylineno, s);
}
