#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"

void printAST(ASTNode* node, int indent) {
    if (!node) return;

    for (int i = 0; i < indent; i++) printf("  ");
    switch (node->type) {
        case AST_VAR_DECL:
            printf("VarDecl: %s %s = \n", node->data.varDecl.varType, node->data.varDecl.name);
            printAST(node->data.varDecl.initializer, indent + 1);
            break;
        case AST_FUNC_DECL:
            printf("FuncDecl: %s %s\n", node->data.funcDecl.returnType, node->data.funcDecl.name);
            printAST(node->data.funcDecl.params, indent + 1);
            printAST(node->data.funcDecl.body, indent + 1);
            break;
        case AST_PARAM:
            printf("Param: %s %s\n", node->data.param.paramType, node->data.param.name);
            break;
        case AST_BLOCK:
            printf("Block\n");
            printAST(node->data.block.declarations, indent + 1);
            break;
        case AST_EXPR_STMT:
            printf("ExprStmt\n");
            printAST(node->data.exprStmt.expression, indent + 1);
            break;
        case AST_BINARY_EXPR:
            printf("BinaryExpr: %c\n", node->data.binaryExpr.operator);
            printAST(node->data.binaryExpr.left, indent + 1);
            printAST(node->data.binaryExpr.right, indent + 1);
            break;
        case AST_LITERAL:
            printf("Literal: %s\n", node->data.literal.value);
            break;
        case AST_IDENTIFIER:
            printf("Identifier: %s\n", node->data.identifier.name);
            break;
        case AST_CALL_EXPR:
            printf("CallExpr: %s\n", node->data.callExpr.callee);
            printAST(node->data.callExpr.arguments, indent + 1);
            break;
    }

    printAST(node->next, indent);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source-file>\n", argv[0]);
        return 1;
    }

    const char* source = argv[1];

    // Initialize the lexer
    initLexer(source);

    // Parse the source code
    ASTNode* ast = parse();

    // Print the AST
    printAST(ast, 0);

    // Free the AST
    freeAST(ast);

    return 0;
}
