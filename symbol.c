#include "symbol.h"

symbol *symbolTableGet(symbol **sym_table, char *name)
{
    for (int i = 0; i < MAX_LIST_SIZE; i++)
    {
        if (sym_table[i] == NULL)
            break;
        if (streql(sym_table[i]->name, name))
            return sym_table[i];
    }
    return NULL;
}

int symbolTableContains(symbol **sym_table, char *name)
{
    return symbolTableGet(sym_table, name) != NULL;
}

// TODO: Maybe I should remove mark argument altogether as it is always 0?
void addToSymbolTable(symbol **sym_table, int next_sym_table_index, int kind, char *name, int val, int addr, int level, int mark)
{
    symbol *sym = malloc(sizeof(symbol));

    sym->kind = kind;
    sym->name = name;
    sym->val = val;
    sym->level = level;
    sym->addr = addr;
    sym->mark = mark;

    sym_table[next_sym_table_index] = sym;
}