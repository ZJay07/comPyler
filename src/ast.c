#include "ast.h"
#include <stdlib.h>
#include <string.h>

ASTNode* newASTNode(ASTNodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    node->next = NULL;
    return node;
}

void freeAST(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case AST_VAR_DECL:
            free(node->data.varDecl.varType);
            free(node->data.varDecl.name);
            freeAST(node->data.varDecl.initializer);
            break;
        case AST_FUNC_DECL:
            free(node->data.funcDecl.returnType);
            free(node->data.funcDecl.name);
            freeAST(node->data.funcDecl.params);
            freeAST(node->data.funcDecl.body);
            break;
        case AST_PARAM:
            free(node->data.param.paramType);
            free(node->data.param.name);
            break;
        case AST_BLOCK:
            freeAST(node->data.block.declarations);
            break;
        case AST_EXPR_STMT:
            freeAST(node->data.exprStmt.expression);
            break;
        case AST_BINARY_EXPR:
            freeAST(node->data.binaryExpr.left);
            freeAST(node->data.binaryExpr.right);
            break;
        case AST_LITERAL:
            free(node->data.literal.value);
            break;
        case AST_IDENTIFIER:
            free(node->data.identifier.name);
            break;
        case AST_CALL_EXPR:
            free(node->data.callExpr.callee);
            freeAST(node->data.callExpr.arguments);
            break;
    }

    freeAST(node->next);
    free(node);
}
