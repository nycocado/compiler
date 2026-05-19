#include "ast.h"

ASTNode* create_node(NodeType type)
{
    ASTNode* node = (ASTNode*)calloc(1, sizeof(ASTNode));
    node->type = type;
    return node;
}

ASTNode* create_id(char* name)
{
    ASTNode* node = create_node(NODE_IDENTIFIER);
    node->value = strdup(name);
    return node;
}

ASTNode* create_const(char* val)
{
    ASTNode* node = create_node(NODE_CONSTANT);
    node->value = strdup(val);
    return node;
}

ASTNode* create_binop(char* op, ASTNode* left, ASTNode* right)
{
    ASTNode* node = create_node(NODE_BINOP);
    node->op = strdup(op);
    node->left = left;
    node->right = right;
    return node;
}

ASTNode* append_node(ASTNode* list, ASTNode* node)
{
    if (!list)
        return node;
    ASTNode* current = list;
    while (current->next)
    {
        current = current->next;
    }
    current->next = node;
    return list;
}

void free_ast(ASTNode* node)
{
    if (!node)
        return;
    free_ast(node->left);
    free_ast(node->middle);
    free_ast(node->right);
    free_ast(node->next);
    if (node->value)
        free(node->value);
    if (node->op)
        free(node->op);
    free(node);
}
