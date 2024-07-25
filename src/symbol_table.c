#include "symbol_table.h"
#include <stdlib.h>
#include <string.h>

SymbolTable* createSymbolTable(SymbolTable *parent) {
    SymbolTable *table = (SymbolTable *)malloc(sizeof(SymbolTable));
    table->head = NULL;
    table->parent = parent;
    return table;
}

void addSymbol(SymbolTable *table, const char *name, const char *type) {
    Symbol *symbol = (Symbol *)malloc(sizeof(Symbol));
    symbol->name = _strdup(name);  // Use _strdup
    symbol->type = _strdup(type);  // Use _strdup
    symbol->scope = 0;  // Define scope level appropriately
    symbol->next = table->head;
    table->head = symbol;
}

Symbol* lookupSymbol(SymbolTable *table, const char *name) {
    for (SymbolTable *current = table; current != NULL; current = current->parent) {
        for (Symbol *sym = current->head; sym != NULL; sym = sym->next) {
            if (strcmp(sym->name, name) == 0) {
                return sym;
            }
        }
    }
    return NULL;  // Not found
}

void freeSymbolTable(SymbolTable *table) {
    Symbol *current = table->head;
    while (current != NULL) {
        Symbol *next = current->next;
        free(current->name);
        free(current->type);
        free(current);
        current = next;
    }
    free(table);
}
