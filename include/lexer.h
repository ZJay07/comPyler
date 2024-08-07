#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_STR,
    TOKEN_FUNC,
    TOKEN_RETURN,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_EQUAL,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_COMMA,
    TOKEN_SEMICOLON,
    TOKEN_STRING,
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    const char* start;
    int length;
    int line;
} Token;

void initLexer(const char* source);
Token scanToken();

#endif // LEXER_H
