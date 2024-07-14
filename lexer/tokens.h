#ifndef TOKENS_H
#define TOKENS_H

typedef enum {
    TOKEN_INT, TOKEN_FLOAT, TOKEN_IDENTIFIER, TOKEN_KEYWORD,
    TOKEN_OPERATOR, TOKEN_PUNCTUATION, TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    char* value;
    int line;
    int column;
} Token;

#endif // TOKENS_H