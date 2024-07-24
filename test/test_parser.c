#include <string.h>
#include "test_framework.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"

void test_var_declaration() {
    const char *source = "int x = 10;";
    initLexer(source);
    ASTNode *ast = parse();

    ASSERT_EQ(AST_BLOCK, ast->type);
    ASSERT_EQ(AST_VAR_DECL, ast->data.block.declarations->type);
    ASSERT_STR_EQ("int", ast->data.block.declarations->data.varDecl.varType);
    ASSERT_STR_EQ("x", ast->data.block.declarations->data.varDecl.name);
    ASSERT_EQ(AST_LITERAL, ast->data.block.declarations->data.varDecl.initializer->type);
    printf("Value: %s\n", ast->data.block.declarations->data.varDecl.initializer->data.literal.value);
    ASSERT_STR_EQ("10", ast->data.block.declarations->data.varDecl.initializer->data.literal.value);

    freeAST(ast);
}

void test_func_declaration() {
    const char *source = "int add(int a, int b) { return a + b; }";
    initLexer(source);
    ASTNode *ast = parse();

    ASSERT_EQ(AST_BLOCK, ast->type);
    ASSERT_EQ(AST_FUNC_DECL, ast->data.block.declarations->type);
    ASSERT_STR_EQ("int", ast->data.block.declarations->data.funcDecl.returnType);
    ASSERT_STR_EQ("add", ast->data.block.declarations->data.funcDecl.name);

    ASTNode *params = ast->data.block.declarations->data.funcDecl.params;
    ASSERT_EQ(AST_PARAM, params->type);
    ASSERT_STR_EQ("int", params->data.param.paramType);
    ASSERT_STR_EQ("a", params->data.param.name);

    params = params->next;
    ASSERT_EQ(AST_PARAM, params->type);
    ASSERT_STR_EQ("int", params->data.param.paramType);
    ASSERT_STR_EQ("b", params->data.param.name);

    ASSERT_EQ(AST_BLOCK, ast->data.block.declarations->data.funcDecl.body->type);

    freeAST(ast);
}

void test_expression_statement() {
    const char *source = "int a = 1; int b=1; a+b;";
    initLexer(source);
    ASTNode *ast = parse();

    ASSERT_EQ(AST_BLOCK, ast->type);
    
    ASTNode* decl = ast->data.block.declarations;
    ASSERT_EQ(AST_VAR_DECL, decl->type);
    ASSERT_STR_EQ("int", decl->data.varDecl.varType);
    ASSERT_STR_EQ("a", decl->data.varDecl.name);
    ASSERT_STR_EQ("1", decl->data.varDecl.initializer->data.literal.value);

    decl = decl->next;
    ASSERT_EQ(AST_VAR_DECL, decl->type);
    ASSERT_STR_EQ("int", decl->data.varDecl.varType);
    ASSERT_STR_EQ("b", decl->data.varDecl.name);
    ASSERT_STR_EQ("1", decl->data.varDecl.initializer->data.literal.value);

    decl = decl->next;
    ASSERT_EQ(AST_EXPR_STMT, decl->type);
    ASSERT_EQ(AST_BINARY_EXPR, decl->data.exprStmt.expression->type);
    ASSERT_STR_EQ("a", decl->data.exprStmt.expression->data.binaryExpr.left->data.identifier.name);
    ASSERT_EQ(TOKEN_PLUS, decl->data.exprStmt.expression->data.binaryExpr.operator);
    ASSERT_STR_EQ("b", decl->data.exprStmt.expression->data.binaryExpr.right->data.identifier.name);

    freeAST(ast);
}


int main() {
    RUN_TEST(test_var_declaration);
    RUN_TEST(test_func_declaration);
    RUN_TEST(test_expression_statement);
    printf("All tests passed.\n");
    return 0;
}
