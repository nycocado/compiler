#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "ast.h"

void translate_ast(ASTNode* node, FILE* out);

#endif
