#include <stdio.h>
#include "lexer.h"

void printToken(Token* token) {
    const char* type;
    switch (token->type) {
        case TOKEN_INT: type = "INT"; break;
        case TOKEN_FLOAT: type = "FLOAT"; break;
        case TOKEN_IDENTIFIER: type = "IDENTIFIER"; break;
        case TOKEN_KEYWORD: type = "KEYWORD"; break;
        case TOKEN_OPERATOR: type = "OPERATOR"; break;
        case TOKEN_PUNCTUATION: type = "PUNCTUATION"; break;
        case TOKEN_EOF: type = "EOF"; break;
        default: type = "UNKNOWN"; break;
    }
    printf("Token(Type: %s, Value: '%s', Line: %d, Column: %d)\n",
           type, token->value, token->line, token->column);
}

int main() {
    const char* sourceCode = "int x = 42;\nfloat y = 3.14;\n";
    Lexer* lexer = createLexer(sourceCode);

    Token* token;
    do {
        token = nextToken(lexer);
        printToken(token);
        destroyToken(token);
    } while (token->type != TOKEN_EOF);

    destroyLexer(lexer);
    return 0;
}
