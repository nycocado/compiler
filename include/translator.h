/**
 * @file translator.h
 * @brief AST to natural language translator
 */

#pragma once

#include "ast.h"

/**
 * @brief Translates the AST to Portuguese natural language.
 * @param node AST root.
 * @param out Output file.
 */
void translate_ast(ASTNode* node, FILE* out);
