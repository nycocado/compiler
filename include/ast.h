#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    NODE_PROGRAM,
    NODE_FUNCTION,
    NODE_PARAM_LIST,
    NODE_PARAM,
    NODE_BLOCK,
    NODE_DECLARATION,
    NODE_ASSIGNMENT,
    NODE_RETURN,
    NODE_IF,
    NODE_WHILE,
    NODE_FOR,
    NODE_DO_WHILE,
    NODE_SWITCH,
    NODE_CASE,
    NODE_CALL,
    NODE_EXPR_LIST,
    NODE_BINOP,
    NODE_UNOP,
    NODE_IDENTIFIER,
    NODE_CONSTANT,
    NODE_CONDITIONAL, // ternary ? :
    NODE_BREAK
} NodeType;

typedef struct ASTNode
{
        NodeType type;
        char* value; // For identifiers, constants, types
        char* op;    // For operators
        struct ASTNode *left, *right, *middle, *next;
} ASTNode;

ASTNode* create_node(NodeType type);
ASTNode* create_id(char* name);
ASTNode* create_const(char* val);
ASTNode* create_binop(char* op, ASTNode* left, ASTNode* right);
ASTNode* append_node(ASTNode* list, ASTNode* node);

void free_ast(ASTNode* node);

#endif
