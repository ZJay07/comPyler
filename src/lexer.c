#include "lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

static const char *start;
static const char *current;
static int line;

void initLexer(const char *source) {
    start = source;
    current = source;
    line = 1;
}

static char advance() {
    return *current++;
}

static char peek() {
    return *current;
}

static char peekNext() {
    if (*current == '\0') return '\0';
    return current[1];
}

static bool isAtEnd() {
    return *current == '\0';
}

static Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = start;
    token.length = (int)(current - start);
    token.line = line;
    return token;
}

static Token errorToken(const char *message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = line;
    return token;
}

static void skipWhitespace() {
    for (;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                line++;
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

static bool match(char expected) {
    if (isAtEnd()) return false;
    if (*current != expected) return false;
    current++;
    return true;
}

static TokenType checkKeyword(int startIdx, int length, const char *rest, TokenType type) {
    if (current - start == startIdx + length && memcmp(start + startIdx, rest, length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}

static TokenType identifierType() {
    switch (start[0]) {
        case 'i': return checkKeyword(1, 2, "nt", TOKEN_INT);
        case 'f': return checkKeyword(1, 3, "unc", TOKEN_FUNC);
        case 's': return checkKeyword(1, 2, "tr", TOKEN_STR);
    }
    return TOKEN_IDENTIFIER;
}

static Token identifier() {
    while (isalnum(peek()) || peek() == '_') advance();
    return makeToken(identifierType());
}

static Token number() {
    while (isdigit(peek())) advance();
    return makeToken(TOKEN_NUMBER);
}

static Token string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated string.");

    advance(); // Closing quote
    return makeToken(TOKEN_STRING);
}

Token scanToken() {
    skipWhitespace();
    start = current;

    if (isAtEnd()) return makeToken(TOKEN_EOF);

    char c = advance();

    if (isalpha(c)) return identifier();
    if (isdigit(c)) return number();

    switch (c) {
        case '+': return makeToken(TOKEN_PLUS);
        case '-': return makeToken(TOKEN_MINUS);
        case '*': return makeToken(TOKEN_STAR);
        case '/': return makeToken(TOKEN_SLASH);
        case '=': return makeToken(TOKEN_EQUAL);
        case '(': return makeToken(TOKEN_LPAREN);
        case ')': return makeToken(TOKEN_RPAREN);
        case '{': return makeToken(TOKEN_LBRACE);
        case '}': return makeToken(TOKEN_RBRACE);
        case ',': return makeToken(TOKEN_COMMA);
        case ';': return makeToken(TOKEN_SEMICOLON);
        case '"': return string();
    }

    return errorToken("Unexpected character.");
}
