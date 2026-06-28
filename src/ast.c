// ============================================================================
// AST (Abstract Syntax Tree) - Helper functions
// ============================================================================
// Implements functions for creating and managing AST nodes.

#include "ast.h"

// ============================================================================
// create_node - Allocate and initialise a new node
// ============================================================================
// Allocates memory for a new AST node and sets its type.
// All fields are zero-initialised by calloc.
ASTNode* create_node(NodeType type)
{
    // calloc zeroes memory (equivalent to malloc + memset)
    ASTNode* node = (ASTNode*)calloc(1, sizeof(ASTNode));
    node->type = type; // set node type
    return node;
}

// ============================================================================
// create_id - Create an identifier node
// ============================================================================
// Creates a NODE_IDENTIFIER node with the variable/function name.
ASTNode* create_id(char* name)
{
    ASTNode* node = create_node(NODE_IDENTIFIER);
    node->value = strdup(name); // duplicate string to avoid ownership issues
    return node;
}

// ============================================================================
// create_const - Create a constant node
// ============================================================================
// Creates a NODE_CONSTANT node with the numeric value.
ASTNode* create_const(char* val)
{
    ASTNode* node = create_node(NODE_CONSTANT);
    node->value = strdup(val); // duplicate value string (e.g. "42", "3.14")
    return node;
}

// ============================================================================
// create_binop - Create a binary operation node
// ============================================================================
// Creates a NODE_BINOP representing an operation with two operands.
// Examples: a + b, x < 10, p && q
ASTNode* create_binop(char* op, ASTNode* left, ASTNode* right)
{
    ASTNode* node = create_node(NODE_BINOP);
    node->op = strdup(op); // operator ("+", "-", "&&", etc.)
    node->left = left;     // left operand
    node->right = right;   // right operand
    return node;
}

// ============================================================================
// append_node - Append a node to the end of a linked list
// ============================================================================
// Links two nodes via the 'next' pointer.
// Used to chain multiple statements, parameters, etc.
ASTNode* append_node(ASTNode* list, ASTNode* node)
{
    // if list is empty, the new node becomes the head
    if (!list)
        return node;

    // walk to the end of the list
    ASTNode* current = list;
    while (current->next)
    {
        current = current->next;
    }

    // append new node at the end
    current->next = node;
    return list; // return unchanged list head
}

// ============================================================================
// set_decl_type - Assign a type to every node in a declarator list
// ============================================================================
ASTNode* set_decl_type(ASTNode* list, char* type)
{
    ASTNode* cur = list;
    while (cur)
    {
        cur->middle = create_node(NODE_IDENTIFIER);
        cur->middle->value = strdup(type);
        cur = cur->next;
    }
    return list;
}

// ============================================================================
// free_ast - Free all memory allocated by the AST
// ============================================================================
// Recursively traverses the tree and frees every node.
// Frees: left, middle, right (recursively) and next (linked list).
void free_ast(ASTNode* node)
{
    // base case: nothing to do
    if (!node)
        return;

    // recursively free children
    free_ast(node->left);   // left child
    free_ast(node->middle); // middle child
    free_ast(node->right);  // right child
    free_ast(node->next);   // next node in list

    // free allocated strings
    if (node->value)
        free(node->value);
    if (node->op)
        free(node->op);

    // free the node itself
    free(node);
}
