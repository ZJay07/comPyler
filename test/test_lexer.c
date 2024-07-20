#include <stdio.h>
#include <string.h>
#include "lexer.h"

void printToken(Token token) {
    printf("Token: Type=%d, Lexeme=%.*s, Line=%d\n", token.type, token.length, token.start, token.line);
}

void runTests() {
    const char *source = "int x = 10;\nfunc add(int a, int b) { return a + b; }";
    initLexer(source);

    Token token;
    do {
        token = scanToken();
        printToken(token);
    } while (token.type != TOKEN_EOF && token.type != TOKEN_ERROR);
}

int main() {
    runTests();
    return 0;
}
