#ifndef SYMBOL_HEADER
#define SYMBOL_HEADER
#include <stdlib.h>
#include "config.h"
#include "lexeme.h"

enum symbol_kind
{
    SYMBOL_CONST = 1,
    SYMBOL_VAR = 2,
    SYMBOL_PROC = 3
};

typedef struct symbol
{
    enum symbol_kind kind;
    char *name;
    int val;   // number (ASCII value)
    int level; // L level
    int addr;  // M address
    int mark;
} symbol;

void printSymbolTable(symbol **table);

symbol *symbolTableGetByName(symbol **sym_table, char *name);
symbol *symbolTableGetProcByValue(symbol **sym_table, int val);
symbol *searchSymbolTableBackwards(symbol **sym_table, char *name, int current_size);

int symbolTableContains(symbol **sym_table, char *name);

void addToSymbolTable(symbol **sym_table, int next_sym_table_index, int kind, char *name, int val, int addr, int level);

#endif