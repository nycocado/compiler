/**
 * @file translator.h
 * @brief Tradutor de AST para linguagem natural
 */

#pragma once

#include "ast.h"

/**
 * @brief Traduz a AST para português
 * @param node Raiz da árvore
 * @param out Arquivo de saída
 */
void translate_ast(ASTNode* node, FILE* out);
