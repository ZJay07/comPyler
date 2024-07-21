#ifndef AST_H
#define AST_H

typedef enum {
    AST_VAR_DECL,
    AST_FUNC_DECL,
    AST_PARAM,
    AST_BLOCK,
    AST_EXPR_STMT,
    AST_BINARY_EXPR,
    AST_LITERAL,
    AST_IDENTIFIER,
    AST_CALL_EXPR
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    struct ASTNode* next;  // For linked list of nodes
    union {
        // Variable declaration
        struct {
            char* varType;
            char* name;
            struct ASTNode* initializer;
        } varDecl;

        // Function declaration
        struct {
            char* returnType;
            char* name;
            struct ASTNode* params;
            struct ASTNode* body;
        } funcDecl;

        // Parameter
        struct {
            char* paramType;
            char* name;
        } param;

        // Block
        struct {
            struct ASTNode* declarations;
        } block;

        // Expression statement
        struct {
            struct ASTNode* expression;
        } exprStmt;

        // Binary expression
        struct {
            struct ASTNode* left;
            char operator;
            struct ASTNode* right;
        } binaryExpr;

        // Literal
        struct {
            char* value;
        } literal;

        // Identifier
        struct {
            char* name;
        } identifier;

        // Call expression
        struct {
            char* callee;
            struct ASTNode* arguments;
        } callExpr;
    } data;
} ASTNode;

ASTNode* newASTNode(ASTNodeType type);
void freeAST(ASTNode* node);

#endif // AST_H
