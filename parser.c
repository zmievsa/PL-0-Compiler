#include <stdlib.h>
#include "symbol.h"
#include "parser.h"
#include "config.h"
#include "lexeme.h"
#include <stdio.h>

#define ltype st->cur_lex->type
#define ldata st->cur_lex->data
#define lline st->cur_lex->line

typedef struct state
{
    symbol **sym_table;
    int sti; // Next sym table index
    lexeme **lex_list;
    int next_lex_list_index;
    lexeme *cur_lex;
} state;

static void factor(state *st, int lex_level);
static void term(state *st, int lex_level);
static void expression(state *st, int lex_level);
static void condition(state *st, int lex_level);
static void statement(state *st, int lex_level);
static int varDeclaration(state *st, int lex_level);
static int constDeclaration(state *st, int lex_level);
static void block(state *st, int lex_level);


static int strToNum(char *str)
{
    static char *endptr;
    return (int)strtol(str, &endptr, 10);
}

static void nextLexeme(state *st) {
    st->cur_lex = st->lex_list[(st->next_lex_list_index)++];
    if (st->cur_lex == NULL)
        error(28, "Unexpectedly ran out of tokens to parse.", st->lex_list[(st->next_lex_list_index - 2)]);
    log("%s", st->cur_lex->data);
}

static void factor(state *st, int lex_level)
{
    elog("factor()");

    if (ltype == IDENTSYM)
    {
        symbol *s = searchSymbolTableBackwardsByKind(st->sym_table, ldata, st->sti, SYMBOL_CONST);
        if (s == NULL) {
            s = searchSymbolTableBackwardsByKind(st->sym_table, ldata, st->sti, SYMBOL_VAR);
            if (s == NULL)
                error(15, "Undeclared identifier.", st->cur_lex);
        }
        nextLexeme(st);
    }
    else if (ltype == NUMBERSYM)
        nextLexeme(st);
    else if (ltype == LPARENTSYM)
    {
        nextLexeme(st);
        expression(st, lex_level);
        if (ltype != RPARENTSYM)
            error(26, "Right parenthesis missing.", st->cur_lex);
        nextLexeme(st);
    }
    else
        error(27, "The preceding factor cannot begin with this symbol.", st->cur_lex);
    elog("/factor()");
}

static void term(state *st, int lex_level)
{
    elog("term()");
    factor(st, lex_level);
    while (ltype == MULTSYM || ltype == SLASHSYM)
    {
        nextLexeme(st);
        factor(st, lex_level);
    }
    elog("/term()");
}

static void expression(state *st, int lex_level)
{
    elog("expression()");
    if (ltype == PLUSSYM || ltype == MINUSSYM)
        nextLexeme(st);
    term(st, lex_level);
    while (ltype == PLUSSYM || ltype == MINUSSYM)
    {
        nextLexeme(st);
        term(st, lex_level);
    }
    elog("/expression()");
}

static void condition(state *st, int lex_level)
{
    elog("condition()");
    if (ltype == ODDSYM)
    {
        nextLexeme(st);
        expression(st, lex_level);
    }
    else
    {
        expression(st, lex_level);
        int t = ltype;
        if (t != EQLSYM && t != NEQSYM && t != LEQSYM && t != LESSYM && t != GTRSYM && t != GEQSYM)
            error(26, "Relational operator expected.", st->cur_lex);
        nextLexeme(st);
        expression(st, lex_level);
    }
    elog("/condition()");
}

static void statement(state *st, int lex_level)
{
    elog("statement()");
    if (ltype == IDENTSYM)
    {
        symbol *sym = searchSymbolTableBackwardsByKind(st->sym_table, ldata, st->sti, SYMBOL_VAR);

        if (sym == NULL)
            error(16, "Expected a variable identifier.", st->cur_lex);
        nextLexeme(st);
        if (ltype != BECOMESSYM)
            error(17, "Assignment operator expected.", st->cur_lex);
        nextLexeme(st);
        expression(st, lex_level);
    }
    else if (ltype == CALLSYM) {
        nextLexeme(st);
		symbol *s = searchSymbolTableBackwardsByKind(st->sym_table, ldata, st->sti, SYMBOL_PROC);
        if (s == NULL)
            error(18, "call must be followed by a procedure identifier.", st->cur_lex);
		nextLexeme(st);
    }
    else if (ltype == BEGINSYM)
    {
        nextLexeme(st);
        statement(st, lex_level);
        while (ltype == SEMICOLONSYM)
        {
            nextLexeme(st);
            statement(st, lex_level);
        }
        if (ltype != ENDSYM)
            error(19, "Semicolon or end expected.", st->cur_lex);
        nextLexeme(st);
    }
    else if (ltype == IFSYM)
    {
        nextLexeme(st);
        condition(st, lex_level);
        if (ltype != THENSYM)
            error(20, "then expected.", st->cur_lex);
        nextLexeme(st);
        if (ltype == ELSESYM)
            nextLexeme(st);
        statement(st, lex_level);
    }
    else if (ltype == THENSYM) {
        error(21, "Then without preceding if is prohibited.", st->cur_lex);
    }
    else if (ltype == WHILESYM)
    {
        nextLexeme(st);
        condition(st, lex_level);
        if (ltype != DOSYM)
            error(22, "do expected.", st->cur_lex);
        nextLexeme(st);
        statement(st, lex_level);
    }
    else if (ltype == READSYM)
    {
        nextLexeme(st);
        if (ltype != IDENTSYM)
            error(23, "Read must be followed by an identifier", st->cur_lex);
        symbol *sym = searchSymbolTableBackwardsByKind(st->sym_table, ldata, st->sti, SYMBOL_VAR);

        if (sym->kind != SYMBOL_VAR)
            error(24, "Read must be followed by a variable identifier.", st->cur_lex);
        nextLexeme(st);
    }
    else if (ltype == WRITESYM)
    {
        nextLexeme(st);
        expression(st, lex_level);
    }
    elog("/statement()");
}

static int procedureDeclaration(state *st, int lex_level) {
    elog("procedureDeclaration()");
	int symCount = 0;
    if (ltype == PROCSYM) {
		do {
			nextLexeme(st);
			if (ltype != IDENTSYM)
				error(13, "procedure must be followed by identifier.", st->cur_lex);
            symbol *s = searchSymbolTableBackwards(st->sym_table, ldata, st->sti);
			if (s != NULL && s->mark == 0 && s->level == lex_level)
				error(8, "The identifier is already defined in current namespace.", st->cur_lex);
            symCount++;
			addToSymbolTable(st->sym_table, st->sti++, 3, ldata, 0, 0, lex_level);
			nextLexeme(st);
			if (ltype != SEMICOLONSYM)
				error(14, "Semicolon between statements missing.", st->cur_lex);
			block(st, lex_level + 1);
			if (ltype != SEMICOLONSYM)
				error(14, "Semicolon between statements missing.", st->cur_lex);
			nextLexeme(st);
        } while (ltype == PROCSYM);
    }
    elog("/procedureDeclaration()");
    return symCount;
}

static int varDeclaration(state *st, int lex_level)
{
    elog("varDeclaration()");
    int varcount = 0;
    if (ltype== VARSYM)
    {
        do
        {
            nextLexeme(st);
            if (ltype != IDENTSYM)
                error(12, "var must be followed by identifier.", st->cur_lex);
            symbol *s = searchSymbolTableBackwards(st->sym_table, ldata, st->sti);
            if (s != NULL && s->mark == 0 && s->level == lex_level)
                error(8, "The identifier is already defined in current namespace.", st->cur_lex);
            varcount++;
            addToSymbolTable(
                st->sym_table, 
                st->sti++, 
                SYMBOL_VAR, 
                ldata, 
                0, 
                varcount + 2, 
                lex_level
            );
            nextLexeme(st);

        } while (ltype == COMMASYM);
        if (ltype != SEMICOLONSYM)
            error(11, "Semicolon or comma missing.", st->cur_lex);
        nextLexeme(st);
    }
    elog("/varDeclaration()");
    return varcount;
}

static int constDeclaration(state *st, int lex_level)
{
    elog("constDeclaration()");
    char *symbol_name;
    int symbol_val;
    int constCount = 0;
    if (ltype == CONSTSYM)
    {
        do
        {
            nextLexeme(st);
            if (ltype != IDENTSYM)
                error(7, "const, var, procedure must be followed by identifier.", st->cur_lex);
            symbol_name = ldata;
            symbol *s = searchSymbolTableBackwards(st->sym_table, symbol_name, st->sti);
            if (s != NULL && s->mark == 0 && s->level == lex_level)
                error(8, "The identifier is already defined in current namespace.", st->cur_lex); 
            nextLexeme(st);
            if (ltype != EQLSYM)
                error(9, "Assignment operator expected.", st->cur_lex);
            nextLexeme(st);
            if (ltype != NUMBERSYM)
                error(10, "= must be followed by a number.", st->cur_lex);
            symbol_val = strToNum(ldata);
            constCount++;
            addToSymbolTable(
                st->sym_table,
                st->sti++,
                SYMBOL_CONST,
                symbol_name,
                symbol_val,
                0,
                lex_level
            );
            nextLexeme(st);
        } while (ltype == COMMASYM);
        if (ltype != SEMICOLONSYM)
            error(11, "Semicolon or comma missing.", st->cur_lex);
        nextLexeme(st);
    }
    elog("/constDeclaration()");
    return constCount;
}

static void block(state *st, int lex_level)
{
    elog("block()");
    nextLexeme(st); // NECESSARY
    int old_sti = st->sti;
    int symCount = 0;
    symCount += constDeclaration(st, lex_level);
    symCount += varDeclaration(st, lex_level);
    symCount += procedureDeclaration(st, lex_level);
    statement(st, lex_level);
    log("\nsymCount: %d\n", symCount);
    for (int i = old_sti; i < symCount + old_sti; i++) {
        symbol *sym = st->sym_table[i];
        sym->mark = 1;
    }
    elog("/block()");
}

symbol **buildSymbolTable(lexeme **lex_list)
{
    elog("buildSymbolTable()");
    symbol **symbol_table = malloc(MAX_LIST_SIZE * sizeof(struct symbol *));
    for (int i = 0; i < MAX_LIST_SIZE; i++)
        symbol_table[i] = NULL;
    state st = {
        .lex_list = lex_list,
        .sym_table = symbol_table,
        .sti = 0,
        .next_lex_list_index = 0,
    };
    
    
    addToSymbolTable(symbol_table, st.sti++, SYMBOL_PROC, "main", 0, 0, 0);
	block(&st, 0);
    if (st.cur_lex->type != PERIODSYM)
        error(6, "Period expected.", st.cur_lex);
    return symbol_table;
}