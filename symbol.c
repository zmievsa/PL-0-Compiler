#include "symbol.h"

// For logging purposes only
void printSymbolTable(symbol **table) {
    elog("printSymbolTable()");
    elog("Lexeme Table:\nsymbol\t\tkind\t\tval\t\tlevel\t\taddr\t\tmark");
    symbol *sym;

    for (int i = 0; i < MAX_LIST_SIZE; i++)
    {
        sym = table[i];
        if (sym == NULL)
            break;
        log("%s\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", sym->name, sym->kind, sym->val, sym->level, sym->addr, sym->mark);
    }
    elog("");
}

symbol *symbolTableGetByName(symbol **sym_table, char *name)
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

symbol *symbolTableGetProcByValue(symbol **sym_table, int val) {
    for (int i = 0; i < MAX_LIST_SIZE; i++)
        {
            if (sym_table[i] == NULL)
                break;
            log("\nsymval=%d, val=%d\n", sym_table[i]->val, val);
            if (sym_table[i]->val == val)
                return sym_table[i];
        }
        return NULL;
}

int symbolTableContains(symbol **sym_table, char *name)
{
    return symbolTableGetByName(sym_table, name) != NULL;
}

// Returns the latest unmarked element
symbol *searchSymbolTableBackwards(symbol **sym_table, char *name, int current_size) {
    for (int i = 1; i <= current_size; i++) {
        symbol *sym = sym_table[current_size - i];
        if (streql(sym->name, name) && sym->mark == 0)
            return sym;
    }
    return NULL;
}

void addToSymbolTable(symbol **sym_table, int next_sym_table_index, int kind, char *name, int val, int addr, int level)
{
    symbol *sym = malloc(sizeof(symbol));

    sym->kind = kind;
    sym->name = name;
    sym->val = val;
    sym->level = level;
    sym->addr = addr;
    sym->mark = 0;

    sym_table[next_sym_table_index] = sym;
}