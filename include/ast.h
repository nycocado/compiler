/**
 * @file ast.h
 * @brief Abstract Syntax Tree node types and structures
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @enum NodeType
 * @brief AST node types
 */
typedef enum
{
    NODE_PROGRAM,     /**< Program */
    NODE_FUNCTION,    /**< Function */
    NODE_PARAM_LIST,  /**< Parameter list */
    NODE_PARAM,       /**< Parameter */
    NODE_BLOCK,       /**< Block */
    NODE_DECLARATION, /**< Declaration */
    NODE_ASSIGNMENT,  /**< Assignment */
    NODE_RETURN,      /**< Return */
    NODE_IF,          /**< If */
    NODE_WHILE,       /**< While */
    NODE_FOR,         /**< For */
    NODE_DO_WHILE,    /**< Do-while */
    NODE_SWITCH,      /**< Switch */
    NODE_CASE,        /**< Case */
    NODE_CALL,        /**< Function call */
    NODE_EXPR_LIST,   /**< Expression list */
    NODE_BINOP,       /**< Binary operator */
    NODE_UNOP,        /**< Unary operator */
    NODE_IDENTIFIER,  /**< Identifier */
    NODE_CONSTANT,    /**< Constant */
    NODE_CONDITIONAL, /**< Ternary operator */
    NODE_BREAK        /**< Break */
} NodeType;

/**
 * @struct ASTNode
 * @brief Abstract Syntax Tree node
 */
typedef struct ASTNode
{
        NodeType type;          /**< Node type */
        char* value;            /**< Value */
        char* op;               /**< Operator */
        struct ASTNode* left;   /**< Left child */
        struct ASTNode* right;  /**< Right child */
        struct ASTNode* middle; /**< Middle child */
        struct ASTNode* next;   /**< Next node in list */
} ASTNode;

/**
 * @brief Allocates and initialises a new node.
 * @param type Node type.
 * @return Newly allocated node.
 */
ASTNode* create_node(NodeType type);

/**
 * @brief Creates an identifier node.
 * @param name Identifier name.
 * @return New NODE_IDENTIFIER node.
 */
ASTNode* create_id(char* name);

/**
 * @brief Creates a constant node.
 * @param val Constant value string.
 * @return New NODE_CONSTANT node.
 */
ASTNode* create_const(char* val);

/**
 * @brief Creates a binary operation node.
 * @param op Operator string.
 * @param left Left operand.
 * @param right Right operand.
 * @return New NODE_BINOP node.
 */
ASTNode* create_binop(char* op, ASTNode* left, ASTNode* right);

/**
 * @brief Appends a node to a linked list.
 * @param list Existing list head.
 * @param node Node to append.
 * @return List head (unchanged).
 */
ASTNode* append_node(ASTNode* list, ASTNode* node);

/**
 * @brief Frees all memory in the AST.
 * @param node AST root.
 */
void free_ast(ASTNode* node);

/**
 * @brief Assigns a type to every node in a declarator list.
 * @param list List of NODE_DECLARATION nodes.
 * @param type Type string to assign.
 * @return List head (unchanged).
 */
ASTNode* set_decl_type(ASTNode* list, char* type);
