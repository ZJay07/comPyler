#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <lexer.h>
#include <stdbool.h>
#include <ast.h>

// Custom implementation of strndup
char* custom_strndup(const char* s, size_t n) {
    char* p = (char*)malloc(n + 1);
    if (p) {
        strncpy_s(p, n + 1, s, n);
        p[n] = '\0';
    }
    return p;
}

static Token currentToken;
static Token previousToken;

// Function prototypes
static void advance();
static void consume(TokenType type, const char* message);
static ASTNode* expression();
static ASTNode* declaration();
static ASTNode* varDeclaration();
static ASTNode* funcDeclaration();
static ASTNode* statement();
static ASTNode* block();
static ASTNode* exprStatement();
static ASTNode* primary();
static ASTNode* parseBinaryExpr(int precedence, ASTNode* left);
static ASTNode* arguments();

static void advance() {
    previousToken = currentToken;
    currentToken = scanToken();
    printf("Advanced to token: Type=%d, Lexeme='%.*s', Line=%d\n", currentToken.type, currentToken.length, currentToken.start, currentToken.line);
}

static bool check(TokenType type) {
    return currentToken.type == type;
}

static bool match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

static void consume(TokenType type, const char* message) {
    if (!match(type)) {
        fprintf(stderr, "Error: %s. Found: Type=%d, Lexeme='%.*s', Line=%d\n", message, currentToken.type, currentToken.length, currentToken.start, currentToken.line);
        exit(1);
    }
}

static ASTNode* newLiteralNode(const char* value) {
    ASTNode* node = newASTNode(AST_LITERAL);
    node->data.literal.value = custom_strndup(value, strlen(value));
    return node;
}

static ASTNode* newIdentifierNode(const char* name) {
    ASTNode* node = newASTNode(AST_IDENTIFIER);
    node->data.identifier.name = custom_strndup(name, strlen(name));
    return node;
}

static ASTNode* primary() {
    if (match(TOKEN_NUMBER)) {
        // Create a new literal node with the value of the token
        return newLiteralNode(previousToken.start);
    }

    if (match(TOKEN_IDENTIFIER)) {
        const char* name = previousToken.start;
        if (match(TOKEN_LPAREN)) {
            ASTNode* node = newASTNode(AST_CALL_EXPR);
            node->data.callExpr.callee = custom_strndup(name, previousToken.length);
            node->data.callExpr.arguments = arguments();
            consume(TOKEN_RPAREN, "Expect ')' after arguments.");
            return node;
        }
        return newIdentifierNode(name);
    }

    if (match(TOKEN_LPAREN)) {
        ASTNode* node = expression();
        consume(TOKEN_RPAREN, "Expect ')' after expression.");
        return node;
    }

    fprintf(stderr, "Error: Unexpected token '%.*s'.\n", currentToken.length, currentToken.start);
    exit(1);
}

static ASTNode* parseBinaryExpr(int precedence, ASTNode* left) {
    ASTNode* node = newASTNode(AST_BINARY_EXPR);
    node->data.binaryExpr.left = left;
    node->data.binaryExpr.operator = previousToken.type;

    // Parse the right-hand side of the binary expression
    node->data.binaryExpr.right = primary();  // Use the appropriate parsing function

    return node;
}

static ASTNode* expression() {
    ASTNode* node = primary();

    while (check(TOKEN_PLUS) || check(TOKEN_MINUS) || check(TOKEN_STAR) || check(TOKEN_SLASH)) {
        char operator = currentToken.type;
        advance();
        node = parseBinaryExpr(operator, node);
    }

    return node;
}

static ASTNode* arguments() {
    ASTNode* node = expression();
    while (match(TOKEN_COMMA)) {
        node->next = expression();
    }
    return node;
}

static ASTNode* varDeclaration() {
    ASTNode* node = newASTNode(AST_VAR_DECL);

    // Capture the variable type
    node->data.varDecl.varType = custom_strndup(previousToken.start, previousToken.length);
    printf("Parsing variable declaration: type='%s'\n", node->data.varDecl.varType);

    // Consume the variable name
    advance();
    if (previousToken.type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expect variable name. Found: Type=%d, Lexeme='%.*s', Line=%d\n", previousToken.type, previousToken.length, previousToken.start, previousToken.line);
        exit(1);
    }
    node->data.varDecl.name = custom_strndup(previousToken.start, previousToken.length);
    printf("Variable name: '%s'\n", node->data.varDecl.name);

    // Consume the '=' token
    printf("Before consuming '=' token: Type=%d, Lexeme='%.*s'\n", currentToken.type, currentToken.length, currentToken.start);
    consume(TOKEN_EQUAL, "Expect '=' after variable name.");
    printf("After consuming '=' token: Type=%d, Lexeme='%.*s'\n", currentToken.type, currentToken.length, currentToken.start);

    // Parse the initializer expression
    node->data.varDecl.initializer = expression();
    printf("Variable initializer parsed\n");

    // Consume the ';' token
    consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");
    printf("Parsed variable declaration: %s %s\n", node->data.varDecl.varType, node->data.varDecl.name);

    return node;
}

static ASTNode* funcDeclaration() {
    ASTNode* node = newASTNode(AST_FUNC_DECL);
    node->data.funcDecl.returnType = custom_strndup(previousToken.start, previousToken.length);
    printf("Parsing function declaration: return type='%s'\n", node->data.funcDecl.returnType);
    consume(TOKEN_IDENTIFIER, "Expect function name.");
    node->data.funcDecl.name = custom_strndup(previousToken.start, previousToken.length);
    printf("Function name: '%s'\n", node->data.funcDecl.name);
    consume(TOKEN_LPAREN, "Expect '(' after function name.");

    // Parse parameters
    if (!check(TOKEN_RPAREN)) {
        node->data.funcDecl.params = newASTNode(AST_PARAM);
        ASTNode* param = node->data.funcDecl.params;
        while (true) {
            consume(TOKEN_INT, "Expect parameter type.");
            param->data.param.paramType = custom_strndup(previousToken.start, previousToken.length);
            consume(TOKEN_IDENTIFIER, "Expect parameter name.");
            param->data.param.name = custom_strndup(previousToken.start, previousToken.length);
            printf("Parameter: %s %s\n", param->data.param.paramType, param->data.param.name);
            if (!match(TOKEN_COMMA)) break;
            param->next = newASTNode(AST_PARAM);
            param = param->next;
        }
    }
    consume(TOKEN_RPAREN, "Expect ')' after parameters.");
    node->data.funcDecl.body = block();
    printf("Parsed function declaration: %s %s\n", node->data.funcDecl.returnType, node->data.funcDecl.name);
    return node;
}

static ASTNode* block() {
    ASTNode* node = newASTNode(AST_BLOCK);
    node->data.block.declarations = NULL;

    consume(TOKEN_LBRACE, "Expect '{' before block.");
    while (!check(TOKEN_RBRACE) && !check(TOKEN_EOF)) {
        if (!node->data.block.declarations) {
            node->data.block.declarations = declaration();
        } else {
            ASTNode* decl = node->data.block.declarations;
            while (decl->next) decl = decl->next;
            decl->next = declaration();
        }
    }
    consume(TOKEN_RBRACE, "Expect '}' after block.");
    return node;
}

static ASTNode* exprStatement() {
    ASTNode* node = newASTNode(AST_EXPR_STMT);
    node->data.exprStmt.expression = expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
    return node;
}

static ASTNode* statement() {
    if (match(TOKEN_LBRACE)) return block();
    return exprStatement();
}

static ASTNode* declaration() {
    if (match(TOKEN_INT) || match(TOKEN_FLOAT) || match(TOKEN_STR)) {
        if (check(TOKEN_IDENTIFIER)) {
            return varDeclaration();
        } else if (check(TOKEN_FUNC)) {
            return funcDeclaration();
        }
    }
    return statement();
}

ASTNode* parse() {
    advance(); // Initialize currentToken
    ASTNode* root = newASTNode(AST_BLOCK);
    root->data.block.declarations = NULL;

    while (!check(TOKEN_EOF)) {
        if (!root->data.block.declarations) {
            root->data.block.declarations = declaration();
        } else {
            ASTNode* decl = root->data.block.declarations;
            while (decl->next) decl = decl->next;
            decl->next = declaration();
        }
    }

    return root;
}
