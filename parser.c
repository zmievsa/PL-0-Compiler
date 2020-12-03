// TODO: Maybe I should store the line with each lexeme to reference it with the error?

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
    log("%s", st->cur_lex->data);
}

static void factor(state *st, int lex_level)
{
    elog("factor()");

    if (ltype == IDENTSYM)
    {
        symbol *s = searchSymbolTableBackwards(st->sym_table, ldata, st->sti);
        if (s == NULL)
            error("Undeclared identifier.", st->cur_lex);
        nextLexeme(st);
    }
    else if (ltype == NUMBERSYM)
        nextLexeme(st);
    else if (ltype == LPARENTSYM)
    {
        nextLexeme(st);
        expression(st, lex_level);
        if (ltype != RPARENTSYM)
            error("Right parenthesis missing.", st->cur_lex);
        nextLexeme(st);
    }
    else
        error("The preceding factor cannot begin with this symbol.", st->cur_lex); // TODO: Not sure about this error message
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
            error("Relational operator expected.", st->cur_lex);
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
        symbol *sym = searchSymbolTableBackwards(st->sym_table, ldata, st->sti);

        if (sym == NULL)
            error("Undeclared identifier.", st->cur_lex);
        if (sym->kind != SYMBOL_VAR)
            error("Expected a variable identifier.", st->cur_lex); // TODO: I made up this message
        nextLexeme(st);
        if (ltype != BECOMESSYM)
            error("Assignment operator expected.", st->cur_lex);
        nextLexeme(st);
        expression(st, lex_level);
    }
    else if (ltype == CALLSYM) {
        nextLexeme(st);
		symbol *s = searchSymbolTableBackwards(st->sym_table, ldata, st->sti);
		if (s == NULL)
            error("Undeclared identifier.", st->cur_lex);
        if (s->kind != SYMBOL_PROC)
            error("Procedure name expected.", st->cur_lex); // TODO: Made up message
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
            error("Incorrect symbol after statement part in block.", st->cur_lex); // TODO: Might be the wrong message
        nextLexeme(st);
    }
    else if (ltype == IFSYM)
    {
        nextLexeme(st);
        condition(st, lex_level);
        if (ltype != THENSYM)
            error("then expected.", st->cur_lex);
        nextLexeme(st);
        if (ltype == ELSESYM)
            nextLexeme(st);
        statement(st, lex_level);
    }
    else if (ltype == THENSYM) {
        error("Then without if encountered", st->cur_lex);
    }
    else if (ltype == WHILESYM)
    {
        nextLexeme(st);
        condition(st, lex_level);
        if (ltype != DOSYM)
            error("do expected.", st->cur_lex);
        nextLexeme(st);
        statement(st, lex_level);
    }
    else if (ltype == READSYM)
    {
        nextLexeme(st);
        if (ltype != IDENTSYM)
            error("Read must be followed by an identifier", st->cur_lex); // TODO: I made up this message
        symbol *sym = searchSymbolTableBackwards(st->sym_table, ldata, st->sti);

        if (sym == NULL)
            error("Undeclared identifier.", st->cur_lex);
        if (sym->kind != SYMBOL_VAR)
            error("Read must be followed by a variable identifier.", st->cur_lex); // TODO: I made up this message
        nextLexeme(st);
    }
    /* Should be replaced by: (according to noelle)
     * if token == write
	 *	nextLexeme(st);
	 *	EXPRESSION (lexlevel)
	 *	return
    */
    else if (ltype == WRITESYM)
    {
        nextLexeme(st);
        if (ltype != IDENTSYM)
            error("Write must be followed by an identifier.", st->cur_lex); // TODO: I made up this message
        if (!symbolTableContains(st->sym_table, st->cur_lex->data))
            error("Undeclared identifier.", st->cur_lex);
        nextLexeme(st);
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
				error("", st->cur_lex);
            symbol *s = symbolTableGetByName(st->sym_table, ldata);
			if (s != NULL && s->mark == 0 && s->level == lex_level)
				error("", st->cur_lex);
            symCount++;
			addToSymbolTable(st->sym_table, st->sti++, 3, ldata, 0, 0, lex_level);
			nextLexeme(st);
			if (ltype != SEMICOLONSYM)
				error("", st->cur_lex);
			// nextLexeme(st); // From noelle's notes
			block(st, lex_level + 1);
			if (ltype != SEMICOLONSYM)
				error("", st->cur_lex);
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
                error("const, var, procedure must be followed by identifier.", st->cur_lex);
            symbol *s = searchSymbolTableBackwards(st->sym_table, ldata, st->sti);
            if (s != NULL && s->mark == 0 && s->level == lex_level)
                error("Redeclaring identifier is not allowed.", st->cur_lex); // TODO: I made this error message because I didn't find a fitting one in the list
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
            error("Semicolon or comma missing.", st->cur_lex);
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
            // TODO: What if we run out of lexemes?
            nextLexeme(st);
            if (ltype != IDENTSYM)
                error("const, var, procedure must be followed by identifier.", st->cur_lex);
            symbol_name = ldata;
            symbol *s = symbolTableGetByName(st->sym_table, symbol_name);
            if (s != NULL && s->mark == 0 && s->level == lex_level)
                error("Assignment to constant or procedure is not allowed.", st->cur_lex); // TODO: This message is most likely incorrect.
            nextLexeme(st);
            if (ltype != EQLSYM)
                error("Assignment operator expected.", st->cur_lex);
            nextLexeme(st);
            if (ltype != NUMBERSYM)
                error("= must be followed by a number.", st->cur_lex); // TODO: Is this a correct message?
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
            error("Semicolon or comma missing.", st->cur_lex);
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
    // mark the last numSymbols number of unmarked symbols
    // TODO: Will most likely need to debug that
    int i;
    log("\nsymcount=%d, old_sti=%d\n", symCount, old_sti);
    for (i = old_sti; i < symCount + old_sti; i++) {
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
        error("Period expected.", st.cur_lex);
    return symbol_table;
}