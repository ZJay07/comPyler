#ifndef SEMANTIC_ANALYSIS_H
#define SEMANTIC_ANALYSIS_H

#include "ast.h"
#include "symbol_table.h"
#include <stdarg.h>

// Function pointer type for custom error handling
typedef void (*ErrorFunction)(const char *, va_list);
void error(const char *format, ...);

// Set the custom error function
void setErrorFunction(ErrorFunction errorFunc);

// Function prototypes
void analyzeProgram(ASTNode *root);
void analyzeNode(ASTNode *node);
void enterScope();
void exitScope();

#endif // SEMANTIC_ANALYSIS_H

