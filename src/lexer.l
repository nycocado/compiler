%{
#include "ast.h"
#include "parser.tab.h"
#include <string.h>

void yyerror(const char *s);
%}

%option noyywrap
%option yylineno

DIGIT       [0-9]
LETTER      [a-zA-Z_]
ID          {LETTER}({LETTER}|{DIGIT})*
NUMBER      {DIGIT}+(\.{DIGIT}+)?

%%

"int"       { yylval.sval = strdup(yytext); return TYPE; }
"float"     { yylval.sval = strdup(yytext); return TYPE; }
"double"    { yylval.sval = strdup(yytext); return TYPE; }
"char"      { yylval.sval = strdup(yytext); return TYPE; }
"void"      { yylval.sval = strdup(yytext); return TYPE; }

"if"        { return IF; }
"else"      { return ELSE; }
"switch"    { return SWITCH; }
"case"      { return CASE; }
"default"   { return DEFAULT; }
"for"       { return FOR; }
"while"     { return WHILE; }
"do"        { return DO; }
"return"    { return RETURN; }
"break"     { return BREAK; }

"=="        { return EQ; }
"!="        { return NE; }
"<="        { return LE; }
">="        { return GE; }
"&&"        { return AND; }
"||"        { return OR; }
"<<"        { return SHL; }
">>"        { return SHR; }

"+="        { return ADD_ASSIGN; }
"-="        { return SUB_ASSIGN; }
"*="        { return MUL_ASSIGN; }
"/="        { return DIV_ASSIGN; }
"%="        { return MOD_ASSIGN; }
"&="        { return AND_ASSIGN; }
"|="        { return OR_ASSIGN; }
"^="        { return XOR_ASSIGN; }
"<<="       { return SHL_ASSIGN; }
">>="       { return SHR_ASSIGN; }

{ID}        { yylval.sval = strdup(yytext); return IDENTIFIER; }
{NUMBER}    { yylval.sval = strdup(yytext); return CONSTANT; }

[ \t\n\r]   { /* ignore whitespace */ }
"//".*      { /* ignore single-line comments */ }
"/*"([^*]|\*+[^*/])*\*+"/" { /* ignore multi-line comments */ }

.           { return yytext[0]; }

%%
