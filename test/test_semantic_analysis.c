#include "semantic_analysis.h"
#include "test_framework.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

// Mock error function to capture errors instead of exiting
static char lastError[256];

void mockError(const char *format, va_list args) {
    vsnprintf(lastError, sizeof(lastError), format, args);
    printf("mockError captured: %s\n", lastError); // Debugging output
}

void test_undeclared_variable() {
    lastError[0] = '\0';
    ASTNode varUse = {.type = AST_IDENTIFIER, .data.identifier.name = "x"};

    setErrorFunction(mockError);
    analyzeNode(&varUse);

    printf("Expected: '%s'\n", "Error: Undeclared identifier 'x'.");
    printf("Actual:   '%s'\n", lastError);

    ASSERT_STR_EQ("Error: Undeclared identifier 'x'.", lastError);
}

void test_redeclaration_in_same_scope() {
    lastError[0] = '\0';
    ASTNode varDecl1 = {.type = AST_VAR_DECL, .data.varDecl.varType = "int", .data.varDecl.name = "x"};
    ASTNode varDecl2 = {.type = AST_VAR_DECL, .data.varDecl.varType = "int", .data.varDecl.name = "x"};

    setErrorFunction(mockError);
    enterScope();

    printf("Analyzing first declaration...\n");
    analyzeNode(&varDecl1);
    printf("First declaration analyzed. lastError: %s\n", lastError);

    printf("Analyzing second declaration...\n");
    analyzeNode(&varDecl2);
    printf("Second declaration analyzed. lastError: %s\n", lastError);

    printf("Expected: '%s'\n", "Error: Variable 'x' already declared.");
    printf("Actual:   '%s'\n", lastError);

    ASSERT_STR_EQ("Error: Variable 'x' already declared.", lastError);

    exitScope();
}


void test_correct_variable_usage() {
    lastError[0] = '\0';
    ASTNode varDecl = {.type = AST_VAR_DECL, .data.varDecl.varType = "int", .data.varDecl.name = "x"};
    ASTNode varUse = {.type = AST_IDENTIFIER, .data.identifier.name = "x"};

    enterScope();
    analyzeNode(&varDecl);
    analyzeNode(&varUse);

    ASSERT_STR_EQ("", lastError);
    exitScope();
}

int main() {
    setErrorFunction(mockError);

    RUN_TEST(test_undeclared_variable);
    RUN_TEST(test_redeclaration_in_same_scope);
    RUN_TEST(test_correct_variable_usage);

    printf("All semantic analysis tests passed.\n");
    return 0;
}
