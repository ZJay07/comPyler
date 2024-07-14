#ifndef LEXER_H
#define LEXER_H

#include "tokens.h"

typedef struct {
    const char* source;
    int length;
    int position;
    int line;
    int column;
} Lexer;

Lexer* createLexer(const char* source);
void destroyLexer(Lexer* lexer);
Token* nextToken(Lexer* lexer);
void destroyToken(Token* token);

#endif // LEXER_H
