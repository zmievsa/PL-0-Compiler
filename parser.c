// TODO: Maybe I should store the line with each lexeme to reference it with the error?

#include <stdlib.h>
#include "symbol.h"
#include "parser.h"
#include "config.h"
#include "lexeme.h"

#define nextLexeme() st->cur_lex = st->lex_list[(st->next_lex_list_index)++]

typedef struct state
{
    symbol **sym_table;
    int next_sym_table_index;
    lexeme **lex_list;
    int next_lex_list_index;
    lexeme *cur_lex;
    int cur_level;
} state;

int strToNum(char *str)
{
    static char *endptr;
    return (int)strtol(str, &endptr, 10);
}

void factor(state *st)
{
    int t = st->cur_lex->type;
    if (t == IDENTSYM)
    {
        if (!symbolTableContains(st->sym_table, st->cur_lex->data))
            error();
        else
            nextLexeme();
    }
    else if (t == NUMBERSYM)
        nextLexeme();
    else if (t == LPARENTSYM)
    {
        nextLexeme();
        expression(st);
        if (t != RPARENTSYM)
            error();
        nextLexeme();
    }
    else
        error();
}

void term(state *st)
{
    factor(st);
    int t = st->cur_lex->type;
    while (t == MULTSYM && t == SLASHSYM)
    {
        nextLexeme();
        factor(st);
    }
}

void expression(state *st)
{
    int t = st->cur_lex->type;
    if (t == PLUSSYM || t == MINUSSYM)
        nextLexeme();
    term(st);
    t = st->cur_lex->type;
    while (t == PLUSSYM || t == MINUSSYM)
    {
        nextLexeme();
        term(st);
    }
}

void condition(state *st)
{
    if (st->cur_lex->type == ODDSYM)
    {
        nextLexeme();
        expression(st);
    }
    else
    {
        expression(st);
        int t = st->cur_lex->type;
        if (t != EQLSYM && t != NEQSYM && t != LEQSYM && t != LESSYM && t != GTRSYM && t != GEQSYM)
            error();
        nextLexeme();
        expression(st);
    }
}

void statement(state *st)
{
    int t = st->cur_lex->type;
    if (t == IDENTSYM)
    {
        symbol *sym = symbolTableGet(st->sym_table, st->cur_lex->data);

        if (sym == NULL)
            error();
        if (sym->kind != SYMBOL_VAR)
            error();
        nextLexeme();
        if (st->cur_lex->type != BECOMESSYM)
            error();
        nextLexeme();
        expression(st);
        return;
    }
    if (t == BEGINSYM)
    {
        nextLexeme();
        statement(st);
        while (st->cur_lex->type == SEMICOLONSYM)
        {
            nextLexeme();
            statement(st);
        }
        if (st->cur_lex->type != ENDSYM)
            error();
        nextLexeme();
        return;
    }
    if (t == IFSYM)
    {
        nextLexeme();
        condition(st);
        if (st->cur_lex->type != THENSYM)
            error();
        nextLexeme();
        statement(st);
        return;
    }
    if (t == WHILESYM)
    {
        nextLexeme();
        condition(st);
        if (st->cur_lex->type != DOSYM)
            error();
        nextLexeme();
        statement(st);
        return;
    }
    if (t == READSYM)
    {
        nextLexeme();
        if (st->cur_lex != IDENTSYM)
            error();
        symbol *sym = symbolTableGet(st->sym_table, st->cur_lex->data);

        if (sym == NULL)
            error();
        if (sym->kind != SYMBOL_VAR)
            error();
        nextLexeme();
        return;
    }
    if (t == WRITESYM)
    {
        nextLexeme();
        if (st->cur_lex != IDENTSYM)
            error();
        if (!symbolTableContains(st->sym_table, st->cur_lex->data))
            error();
        nextLexeme();
        return;
    }
    return;
}

void varDeclaration(state *st)
{
    if (st->cur_lex->type == VARSYM)
    {
        int varcount = 0;
        do
        {
            varcount++;
            nextLexeme();
            if (st->cur_lex->type != IDENTSYM)
                error();
            if (symbolTableContains(st->sym_table, st->cur_lex->data))
                error();
            addToSymbolTable(st->sym_table, st->next_sym_table_index++, SYMBOL_VAR, st->cur_lex->data, 0, varcount + 2, st->cur_level, 0);
            nextLexeme();

        } while (st->cur_lex->type == COMMASYM);
        if (st->cur_lex->type != SEMICOLONSYM)
            error();
        nextLexeme();
    }
}

void constDeclaration(state *st)
{
    char *symbol_name;
    int symbol_val;
    if (st->cur_lex->type == CONSTSYM)
    {
        do
        {
            // TODO: What if we run out of lexemes?
            nextLexeme();
            if (st->cur_lex->type != IDENTSYM)
                error();
            symbol_name = st->cur_lex->data;
            if (symbolTableContains(st->sym_table, st->cur_lex->data))
                error();
            nextLexeme();
            if (st->cur_lex->type != EQLSYM)
                error();
            nextLexeme();
            if (st->cur_lex->type != NUMBERSYM)
                error();
            symbol_val = strToNum(st->cur_lex->data);
            addToSymbolTable(st->sym_table, st->next_sym_table_index++, SYMBOL_CONST, symbol_name, symbol_val, 0, st->cur_level, 0);
            nextLexeme();
        } while (st->cur_lex->type == COMMASYM);
        if (st->cur_lex->type != SEMICOLONSYM)
            error();
        nextLexeme();
    }
}

void block(state *st)
{
    nextLexeme();
    constDeclaration(st);
    varDeclaration(st);
}

symbol **buildSymbolTable(lexeme **lex_list)
{
    symbol *symbol_table[MAX_LIST_SIZE] = {NULL};
    state st = {
        .cur_level = 0,
        .lex_list = lex_list,
        .sym_table = symbol_table,
        .next_sym_table_index = 0,
        .next_lex_list_index = 0};

    block(&st);
    if (st.cur_lex != PERIODSYM)
        error();
    return symbol_table;
}