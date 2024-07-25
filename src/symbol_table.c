#include "symbol_table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

SymbolTable* createSymbolTable(SymbolTable *parent) {
    SymbolTable *table = (SymbolTable *)malloc(sizeof(SymbolTable));
    table->head = NULL;
    table->parent = parent;
    return table;
}

void addSymbol(SymbolTable *table, const char *name, const char *type) {
    Symbol *sym = (Symbol *)malloc(sizeof(Symbol));
    sym->name = _strdup(name);
    sym->type = _strdup(type);
    sym->next = table->head;
    table->head = sym;
    printf("Added symbol: %s\n", name); // Debugging
}

Symbol* lookupSymbol(SymbolTable *table, const char *name) {
    printf("lookupSymbol: Looking for %s\n", name); // Debugging
    while (table) {
        for (Symbol *sym = table->head; sym != NULL; sym = sym->next) {
            printf("Checking symbol: %s\n", sym->name); // Debugging
            if (strcmp(sym->name, name) == 0) {
                printf("Symbol found: %s\n", sym->name); // Debugging
                return sym;
            }
        }
        table = table->parent;
    }
    printf("Symbol not found: %s\n", name); // Debugging
    return NULL;
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
