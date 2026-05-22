/**
 * @file ast.h
 * @brief Estruturas de Árvore de Sintaxe Abstrata
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @enum NodeType
 * @brief Tipos de nós da AST
 */
typedef enum
{
    NODE_PROGRAM,      /**< Programa */
    NODE_FUNCTION,     /**< Função */
    NODE_PARAM_LIST,   /**< Lista de parâmetros */
    NODE_PARAM,        /**< Parâmetro */
    NODE_BLOCK,        /**< Bloco */
    NODE_DECLARATION,  /**< Declaração */
    NODE_ASSIGNMENT,   /**< Atribuição */
    NODE_RETURN,       /**< Return */
    NODE_IF,           /**< If */
    NODE_WHILE,        /**< While */
    NODE_FOR,          /**< For */
    NODE_DO_WHILE,     /**< Do-while */
    NODE_SWITCH,       /**< Switch */
    NODE_CASE,         /**< Case */
    NODE_CALL,         /**< Chamada de função */
    NODE_EXPR_LIST,    /**< Lista de expressões */
    NODE_BINOP,        /**< Operador binário */
    NODE_UNOP,         /**< Operador unário */
    NODE_IDENTIFIER,   /**< Identificador */
    NODE_CONSTANT,     /**< Constante */
    NODE_CONDITIONAL,  /**< Operador ternário */
    NODE_BREAK         /**< Break */
} NodeType;

/**
 * @struct ASTNode
 * @brief Nó da Árvore de Sintaxe Abstrata
 */
typedef struct ASTNode
{
    NodeType type;              /**< Tipo do nó */
    char* value;                /**< Valor */
    char* op;                   /**< Operador */
    struct ASTNode *left;       /**< Filho esquerdo */
    struct ASTNode *right;      /**< Filho direito */
    struct ASTNode *middle;     /**< Filho do meio */
    struct ASTNode *next;       /**< Próximo nó */
} ASTNode;

/**
 * @brief Cria um novo nó
 * @param type Tipo do nó
 * @return Novo nó alocado
 */
ASTNode* create_node(NodeType type);

/**
 * @brief Cria um nó identificador
 * @param name Nome do identificador
 * @return Novo nó NODE_IDENTIFIER
 */
ASTNode* create_id(char* name);

/**
 * @brief Cria um nó constante
 * @param val Valor da constante
 * @return Novo nó NODE_CONSTANT
 */
ASTNode* create_const(char* val);

/**
 * @brief Cria um nó de operação binária
 * @param op Operador
 * @param left Operando esquerdo
 * @param right Operando direito
 * @return Novo nó NODE_BINOP
 */
ASTNode* create_binop(char* op, ASTNode* left, ASTNode* right);

/**
 * @brief Adiciona um nó a uma lista ligada
 * @param list Lista existente
 * @param node Nó a adicionar
 * @return Início da lista
 */
ASTNode* append_node(ASTNode* list, ASTNode* node);

/**
 * @brief Libera a memória da árvore
 * @param node Raiz da árvore
 */
void free_ast(ASTNode* node);

/**
 * @brief Define o tipo numa lista de declaradores
 * @param list Lista de NODE_DECLARATION
 * @param type Tipo a atribuir
 * @return Início da lista
 */
ASTNode* set_decl_type(ASTNode* list, char* type);
