#include "semantic_analysis.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static ErrorFunction customErrorFunction = NULL;

void setErrorFunction(ErrorFunction errorFunc) {
    customErrorFunction = errorFunc;
}

void error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    if (customErrorFunction) {
        customErrorFunction(format, args);
    } else {
        vfprintf(stderr, format, args);
    }
    va_end(args);
}

static SymbolTable *currentScope = NULL;

void enterScope() {
    currentScope = createSymbolTable(currentScope);
}

void exitScope() {
    SymbolTable *oldScope = currentScope;
    currentScope = currentScope->parent;
    freeSymbolTable(oldScope);
}

void analyzeVariableDeclaration(ASTNode *node) {
    Symbol *sym = lookupSymbol(currentScope, node->data.varDecl.name);
    if (sym) {
        printf("Error triggered for variable '%s' already declared.\n", node->data.varDecl.name); // Debugging
        error("Error: Variable '%s' already declared.", node->data.varDecl.name);
        return;
    }
    addSymbol(currentScope, node->data.varDecl.name, node->data.varDecl.varType);
}

void analyzeExpression(ASTNode *node) {
    if (node->type == AST_IDENTIFIER) {
        Symbol *symbol = lookupSymbol(currentScope, node->data.identifier.name);
        if (!symbol) {
            error("Error: Undeclared identifier '%s'.", node->data.identifier.name); // Removed \n
        }
    }
}

void analyzeNode(ASTNode *node) {
    if (node == NULL) return;

    switch (node->type) {
        case AST_VAR_DECL:
            printf("Analyzing variable declaration: %s\n", node->data.varDecl.name); // Debugging
            analyzeVariableDeclaration(node);
            break;
        case AST_BLOCK:
            enterScope();
            for (ASTNode *decl = node->data.block.declarations; decl != NULL; decl = decl->next) {
                analyzeNode(decl);
            }
            exitScope();
            break;
        case AST_FUNC_DECL:
            // Add function analysis logic here
            break;
        default:
            analyzeExpression(node);
            break;
    }
}

void analyzeProgram(ASTNode *root) {
    enterScope();
    analyzeNode(root);
    exitScope();
}
