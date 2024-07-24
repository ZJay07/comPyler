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
static ASTNode* varDeclaration(TokenType type);
static ASTNode* funcDeclaration(TokenType type);
static ASTNode* statement();
static ASTNode* block();
static ASTNode* exprStatement();
static ASTNode* returnStatement();
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

static ASTNode* newLiteralNode(const char* value, int length) {
    ASTNode* node = newASTNode(AST_LITERAL);
    node->data.literal.value = custom_strndup(value, length);
    return node;
}

static ASTNode* newIdentifierNode(const char* name, int length) {
    ASTNode* node = newASTNode(AST_IDENTIFIER);
    node->data.identifier.name = custom_strndup(name, length);
    return node;
}

static ASTNode* primary() {
    if (match(TOKEN_NUMBER)) {
        return newLiteralNode(previousToken.start, previousToken.length);
    }

    if (match(TOKEN_IDENTIFIER)) {
        return newIdentifierNode(previousToken.start, previousToken.length);
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
    while (true) {
        // Capture the current operator type
        TokenType operatorType = currentToken.type;

        // Check if it's a binary operator
        if (operatorType != TOKEN_PLUS && operatorType != TOKEN_MINUS &&
            operatorType != TOKEN_STAR && operatorType != TOKEN_SLASH) {
            return left; // If not an operator, return the left operand
        }

        printf("Operator found: %.*s\n", currentToken.length, currentToken.start);

        advance(); // Consume the operator

        // Parse the right-hand side expression
        ASTNode* right = primary();
        if (!right) {
            fprintf(stderr, "Error: Failed to parse right operand of binary expression.\n");
            exit(1);
        }

        // Create a new binary expression node
        ASTNode* node = newASTNode(AST_BINARY_EXPR);
        node->data.binaryExpr.left = left;
        node->data.binaryExpr.operator = operatorType; // Set the operator
        node->data.binaryExpr.right = right;

        printf("Binary expression parsed: left='%s', operator='%d', right='%s'\n",
               left->data.identifier.name, operatorType, right->data.identifier.name);

        left = node; // Update left to the new binary expression node
    }
}

static ASTNode* expression() {
    return parseBinaryExpr(0, primary());
}

static ASTNode* arguments() {
    ASTNode* node = expression();
    while (match(TOKEN_COMMA)) {
        node->next = expression();
    }
    return node;
}

static ASTNode* varDeclaration(TokenType type) {
    ASTNode* node = newASTNode(AST_VAR_DECL);

    // Set the variable type from the captured type token
    if (type == TOKEN_INT) {
        node->data.varDecl.varType = custom_strndup("int", 3);
    } else if (type == TOKEN_FLOAT) {
        node->data.varDecl.varType = custom_strndup("float", 5);
    } else if (type == TOKEN_STR) {
        node->data.varDecl.varType = custom_strndup("str", 3);
    }

    printf("Parsing variable declaration: type='%s'\n", node->data.varDecl.varType);

    // The previous token is the variable name
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

static ASTNode* funcDeclaration(TokenType type) {
    ASTNode* node = newASTNode(AST_FUNC_DECL);

    // Set the function return type from the captured type token
    if (type == TOKEN_INT) {
        node->data.funcDecl.returnType = custom_strndup("int", 3);
    } else if (type == TOKEN_FLOAT) {
        node->data.funcDecl.returnType = custom_strndup("float", 5);
    } else if (type == TOKEN_STR) {
        node->data.funcDecl.returnType = custom_strndup("str", 3);
    }

    printf("Parsing function declaration: return type='%s'\n", node->data.funcDecl.returnType);

    // Consume the function name
    if (previousToken.type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expect function name. Found: Type=%d, Lexeme='%.*s', Line=%d\n", previousToken.type, previousToken.length, previousToken.start, previousToken.line);
        exit(1);
    }
    node->data.funcDecl.name = custom_strndup(previousToken.start, previousToken.length);
    printf("Function name: '%s'\n", node->data.funcDecl.name);
    
    consume(TOKEN_LPAREN, "Expect '(' after function name.");

    // Parse parameters
    if (!check(TOKEN_RPAREN)) {
        node->data.funcDecl.params = newASTNode(AST_PARAM);
        ASTNode* param = node->data.funcDecl.params;
        while (true) {
            advance();
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

static ASTNode* returnStatement() {
    ASTNode* node = newASTNode(AST_RETURN_STMT);
    if (!check(TOKEN_SEMICOLON)) {
        node->data.returnStmt.value = expression();
    }
    consume(TOKEN_SEMICOLON, "Expect ';' after return value.");
    return node;
}

static ASTNode* statement() {
    if (match(TOKEN_RETURN)) return returnStatement();
    if (match(TOKEN_LBRACE)) return block();
    return exprStatement();
}

static ASTNode* declaration() {
    if (match(TOKEN_INT) || match(TOKEN_FLOAT) || match(TOKEN_STR)) {
        TokenType type = previousToken.type; // Capture the type token
        if (check(TOKEN_IDENTIFIER)) {
            advance(); // Advance to the identifier
            if (check(TOKEN_LPAREN)) {
                return funcDeclaration(type); // Pass the type token
            } else {
                return varDeclaration(type); // Pass the type token
            }
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
