#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"

// Helper functions
void advance(Lexer* lexer) {
    if (lexer->source[lexer->position] == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    lexer->position++;
}

char peek(Lexer* lexer) {
    return lexer->source[lexer->position];
}

char peekNext(Lexer* lexer) {
    return lexer->position + 1 < lexer->length ? lexer->source[lexer->position + 1] : '\0';
}

Token* createToken(TokenType type, const char* value, int line, int column) {
    Token* token = (Token*)malloc(sizeof(Token));
    if (!token) {
        fprintf(stderr, "Memory allocation failed for token.\n");
        exit(EXIT_FAILURE);
    }
    token->type = type;
    token->value = strdup(value);
    token->line = line;
    token->column = column;
    return token;
}

Lexer* createLexer(const char* source) {
    Lexer* lexer = (Lexer*)malloc(sizeof(Lexer));
    if (!lexer) {
        fprintf(stderr, "Memory allocation failed for lexer.\n");
        exit(EXIT_FAILURE);
    }
    lexer->source = source;
    lexer->length = strlen(source);
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    return lexer;
}

void destroyLexer(Lexer* lexer) {
    if (lexer) {
        free(lexer);
    }
}

void destroyToken(Token* token) {
    if (token) {
        if (token->value) {
            free(token->value);
        }
        free(token);
    }
}

Token* nextToken(Lexer* lexer) {
    while (lexer->position < lexer->length) {
        char current = peek(lexer);

        // Skip whitespace
        if (isspace(current)) {
            advance(lexer);
            continue;
        }

        // Handle identifiers and keywords
        if (isalpha(current) || current == '_') {
            int start = lexer->position;
            while (isalnum(peek(lexer)) || peek(lexer) == '_') {
                advance(lexer);
            }
            int length = lexer->position - start;
            char* value = (char*)malloc(length + 1);
            if (!value) {
                fprintf(stderr, "Memory allocation failed for identifier.\n");
                exit(EXIT_FAILURE);
            }
            strncpy(value, lexer->source + start, length);
            value[length] = '\0';
            Token* token = createToken(TOKEN_IDENTIFIER, value, lexer->line, lexer->column - length);
            free(value);
            return token;
        }

        // Handle numeric literals
        if (isdigit(current)) {
            int start = lexer->position;
            while (isdigit(peek(lexer))) {
                advance(lexer);
            }
            if (peek(lexer) == '.' && isdigit(peekNext(lexer))) {
                advance(lexer);
                while (isdigit(peek(lexer))) {
                    advance(lexer);
                }
                int length = lexer->position - start;
                char* value = (char*)malloc(length + 1);
                if (!value) {
                    fprintf(stderr, "Memory allocation failed for float literal.\n");
                    exit(EXIT_FAILURE);
                }
                strncpy(value, lexer->source + start, length);
                value[length] = '\0';
                Token* token = createToken(TOKEN_FLOAT, value, lexer->line, lexer->column - length);
                free(value);
                return token;
            } else {
                int length = lexer->position - start;
                char* value = (char*)malloc(length + 1);
                if (!value) {
                    fprintf(stderr, "Memory allocation failed for integer literal.\n");
                    exit(EXIT_FAILURE);
                }
                strncpy(value, lexer->source + start, length);
                value[length] = '\0';
                Token* token = createToken(TOKEN_INT, value, lexer->line, lexer->column - length);
                free(value);
                return token;
            }
        }

        // Handle single-character tokens
        switch (current) {
            case '+': case '-': case '*': case '/': case '=': 
            case ';': case '(': case ')':
                {
                    char value[2] = {current, '\0'};
                    Token* token = createToken(TOKEN_OPERATOR, value, lexer->line, lexer->column);
                    advance(lexer);
                    return token;
                }
            default:
                fprintf(stderr, "Unexpected character '%c' at line %d, column %d.\n", current, lexer->line, lexer->column);
                advance(lexer);
                break;
        }
    }

    return createToken(TOKEN_EOF, "EOF", lexer->line, lexer->column);
}
