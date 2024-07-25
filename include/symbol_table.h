#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

typedef struct Symbol {
    char *name;
    char *type;
    int scope;  // For simplicity, using an integer to denote scope level
    struct Symbol *next;
} Symbol;

typedef struct SymbolTable {
    Symbol *head;
    struct SymbolTable *parent;  // For nested scopes
} SymbolTable;

SymbolTable* createSymbolTable(SymbolTable *parent);
void addSymbol(SymbolTable *table, const char *name, const char *type);
Symbol* lookupSymbol(SymbolTable *table, const char *name);
void freeSymbolTable(SymbolTable *table);

#endif // SYMBOL_TABLE_H
