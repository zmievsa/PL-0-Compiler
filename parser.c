// TODO: Maybe I should store the line with each lexeme to reference it with the error?

#include <stdlib.h>
#include "symbol.h"
#include "parser.h"
#include "config.h"
#include "lexeme.h"
#include <stdio.h>

#define error(...) {printf("\n" __VA_ARGS__); printf("\n"); exit(1);}
#define ltype st->cur_lex->type

typedef struct state
{
    symbol **sym_table;
    int next_sym_table_index;
    lexeme **lex_list;
    int next_lex_list_index;
    lexeme *cur_lex;
    int cur_level;
} state;

static void factor(state *st);
static void term(state *st);
static void expression(state *st);
static void condition(state *st);
static void statement(state *st);
static void varDeclaration(state *st);
static void constDeclaration(state *st);
static void block(state *st);

static int strToNum(char *str)
{
    static char *endptr;
    return (int)strtol(str, &endptr, 10);
}

static void nextLexeme(state *st) {
    st->cur_lex = st->lex_list[(st->next_lex_list_index)++];
    log("%s", st->cur_lex->data);
}

static void factor(state *st)
{
    elog("factor()");

    if (ltype == IDENTSYM)
    {
        if (!symbolTableContains(st->sym_table, st->cur_lex->data))
            error("Undeclared identifier.")
        else
            nextLexeme(st);
    }
    else if (ltype == NUMBERSYM)
        nextLexeme(st);
    else if (ltype == LPARENTSYM)
    {
        nextLexeme(st);
        expression(st);
        if (ltype != RPARENTSYM)
            error("Right parenthesis missing.")
        nextLexeme(st);
    }
    else
        error("The preceding factor cannot begin with this symbol.") // TODO: Not sure about this error message
    elog("/factor()");
}

static void term(state *st)
{
    elog("term()");
    factor(st);
    while (ltype == MULTSYM || ltype == SLASHSYM)
    {
        nextLexeme(st);
        factor(st);
    }
    elog("/term()");
}

static void expression(state *st)
{
    elog("expression()");
    if (ltype == PLUSSYM || ltype == MINUSSYM)
        nextLexeme(st);
    term(st);
    while (ltype == PLUSSYM || ltype == MINUSSYM)
    {
        nextLexeme(st);
        term(st);
    }
    elog("/expression()");
}

static void condition(state *st)
{
    elog("condition()");
    if (ltype == ODDSYM)
    {
        nextLexeme(st);
        expression(st);
    }
    else
    {
        expression(st);
        int t = ltype;
        if (t != EQLSYM && t != NEQSYM && t != LEQSYM && t != LESSYM && t != GTRSYM && t != GEQSYM)
            error("Relational operator expected.")
        nextLexeme(st);
        expression(st);
    }
    elog("/condition()");
}

static void statement(state *st)
{
    elog("statement()");
    if (ltype == IDENTSYM)
    {
        symbol *sym = symbolTableGet(st->sym_table, st->cur_lex->data);

        if (sym == NULL)
            error("Undeclared identifier.")
        if (sym->kind != SYMBOL_VAR)
            error("Expected a variable identifier.") // TODO: I made up this message
        nextLexeme(st);
        if (st->cur_lex->type != BECOMESSYM)
            error("Assignment operator expected.")
        nextLexeme(st);
        expression(st);
    }
    else if (ltype == BEGINSYM)
    {
        nextLexeme(st);
        statement(st);
        while (st->cur_lex->type == SEMICOLONSYM)
        {
            nextLexeme(st);
            statement(st);
        }
        if (st->cur_lex->type != ENDSYM)
            error("Incorrect symbol after statement part in block.") // TODO: Might be the wrong message
        nextLexeme(st);
    }
    else if (ltype == IFSYM)
    {
        nextLexeme(st);
        condition(st);
        if (st->cur_lex->type != THENSYM)
            error("then expected.")
        nextLexeme(st);
        statement(st);
    }
    else if (ltype == WHILESYM)
    {
        nextLexeme(st);
        condition(st);
        if (st->cur_lex->type != DOSYM)
            error("do expected.")
        nextLexeme(st);
        statement(st);
    }
    else if (ltype == READSYM)
    {
        nextLexeme(st);
        if (ltype != IDENTSYM)
            error("Read must be followed by an identifier") // TODO: I made up this message
        symbol *sym = symbolTableGet(st->sym_table, st->cur_lex->data);

        if (sym == NULL)
            error("Undeclared identifier.")
        if (sym->kind != SYMBOL_VAR)
            error("Read must be followed by a variable identifier.") // TODO: I made up this message
        nextLexeme(st);
    }
    else if (ltype == WRITESYM)
    {
        nextLexeme(st);
        if (ltype != IDENTSYM)
            error("Write must be followed by an identifier.") // TODO: I made up this message
        if (!symbolTableContains(st->sym_table, st->cur_lex->data))
            error("Undeclared identifier.")
        nextLexeme(st);
    }
    elog("/statement()");
}

static void varDeclaration(state *st)
{
    elog("varDeclaration()");
    if (st->cur_lex->type == VARSYM)
    {
        int varcount = 0;
        do
        {
            varcount++;
            nextLexeme(st);
            if (st->cur_lex->type != IDENTSYM)
                error("const, var, procedure must be followed by identifier.")
            if (symbolTableContains(st->sym_table, st->cur_lex->data))
                error("Redeclaring identifier is not allowed.") // TODO: I made this error message because I didn't find a fitting one in the list
            addToSymbolTable(st->sym_table, st->next_sym_table_index++, SYMBOL_VAR, st->cur_lex->data, 0, varcount + 2, st->cur_level, 0);
            nextLexeme(st);

        } while (st->cur_lex->type == COMMASYM);
        if (st->cur_lex->type != SEMICOLONSYM)
            error("Semicolon or comma missing.")
        nextLexeme(st);
    }
    elog("/varDeclaration()");
}

static void constDeclaration(state *st)
{
    elog("constDeclaration()");
    char *symbol_name;
    int symbol_val;
    if (st->cur_lex->type == CONSTSYM)
    {
        do
        {
            // TODO: What if we run out of lexemes?
            nextLexeme(st);
            if (st->cur_lex->type != IDENTSYM)
                error("const, var, procedure must be followed by identifier.")
            symbol_name = st->cur_lex->data;
            if (symbolTableContains(st->sym_table, st->cur_lex->data))
                error("Assignment to constant or procedure is not allowed.")
            nextLexeme(st);
            if (st->cur_lex->type != EQLSYM)
                error("Assignment operator expected.")
            nextLexeme(st);
            if (st->cur_lex->type != NUMBERSYM)
                error("= must be followed by a number.")
            symbol_val = strToNum(st->cur_lex->data);
            addToSymbolTable(st->sym_table, st->next_sym_table_index++, SYMBOL_CONST, symbol_name, symbol_val, 0, st->cur_level, 0);
            nextLexeme(st);
        } while (st->cur_lex->type == COMMASYM);
        if (st->cur_lex->type != SEMICOLONSYM)
            error("Semicolon or comma missing.")
        nextLexeme(st);
    }
    elog("/constDeclaration()");
}

static void block(state *st)
{
    elog("block()");
    nextLexeme(st); // NECESSARY
    constDeclaration(st);
    varDeclaration(st);
    statement(st);
    elog("/block()");
}

symbol **buildSymbolTable(lexeme **lex_list)
{
    elog("buildSymbolTable()");
    symbol **symbol_table = malloc(MAX_LIST_SIZE * sizeof(struct symbol *));
    for (int i = 0; i < MAX_LIST_SIZE; i++)
        symbol_table[i] = NULL;
    state st = {
        .cur_level = 0,
        .lex_list = lex_list,
        .sym_table = symbol_table,
        .next_sym_table_index = 0,
        .next_lex_list_index = 0};

    block(&st);
    if (st.cur_lex->type != PERIODSYM)
        error("Period expected.")
    return symbol_table;
}