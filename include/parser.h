#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

ASTNode* parse();
void initLexer(const char* source);

#endif // PARSER_H
