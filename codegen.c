#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"
#include "codegen.h"
#include "config.h"

#define ltype st->cl->type
#define ldata st->cl->data
#define lline st->cl->line
// Gets variable/const name from the next lexeme (assuming we're one past the VAR or CONST lexeme rn)
#define getVarName() st->cl->data

// Gets variable/const value from the next lexeme (assuming we're on the VAR or CONST lexeme rn)
#define curVar symbolTableGet(st->sym_table, getVarName())
#define logInstr(i) log("\nemit: %s\t%d\t%d\t%d\n", i->op_name, i->r, i->l, i->m)

typedef struct state
{
	symbol **sym_table;
	int sti; // Next sym_table index
	lexeme **lex_list;
	int lli; // Next lex_list index
	// lexeme *cur_lex;
	instruction **code;
	int ci;		// Next code index
	lexeme *cl; // Current lex
} state;

static void factor(state *st, int regtoendupin, int lex_level);
static void term(state *st, int regtoendupin, int lex_level);
static void expression(state *st, int regtoendupin, int lex_level);
static void condition(state *st, int lex_level);
static void statement(state *st, int lex_level);
static int varDeclaration(state *st, int lex_level);
static int constDeclaration(state *st, int lex_level);
static void block(state *st, int lex_level, symbol* cur_procedure);

static void nextLexeme(state *st) {
    st->cl = st->lex_list[(st->lli)++];
    log("%s", st->cl->data);
}

static void emit(state *st, int op, int r, int l, int m)
{
	if (st->ci > MAX_LIST_SIZE) {
		printf("TOO MUCH CODE");
		exit(1);
	}
	else
	{
		instruction *instr = malloc(sizeof(instruction));
		instr->op = op;
		instr->op_name = opname(op);
		instr->r = r;
		instr->l = l;
		instr->m = m;
		st->code[st->ci++] = instr;
		// logInstr(instr);
	}
}
static int strToNum(char *str)
{
	static char *endptr;
	return (int)strtol(str, &endptr, 10);
}


symbol *symbolTableGetByNameAndLevel(symbol **sym_table, char *name, int level)
{
    for (int i = 0; i < MAX_LIST_SIZE; i++)
    {
		symbol *sym = sym_table[i];
        if (sym == NULL)
            break;
        if (sym->level == level && streql(sym->name, name))
            return sym_table[i];
    }
    return NULL;
}


symbol *symbolTableGetByNameAndClosestLevel(symbol **sym_table, char *name, int level) {
	symbol *closestByLevel = NULL;
	symbol *sym;
	for (int i = 0; i < MAX_LIST_SIZE; i++) {
		sym = sym_table[i];
		if (sym == NULL)
			break;
		if (streql(sym->name, name)) {
			if (closestByLevel == NULL || (abs(sym->level - level) < abs(closestByLevel->level - level)))
				if (sym->mark == 0)
					closestByLevel = sym;
		} 
	}
	return closestByLevel;
}


static void factor(state *st, int regtoendupin, int lex_level)
{
	elog("factor() {");
	symbol *sym;
	if (ltype == IDENTSYM)
	{
		sym = symbolTableGetByNameAndClosestLevel(st->sym_table, ldata, lex_level);
		if (sym->kind == SYMBOL_CONST)
			emit(st, LIT, regtoendupin, 0, sym->val);
		else if (sym->kind == SYMBOL_VAR)
			emit(st, LOD, regtoendupin, lex_level - sym->level, sym->addr);
		nextLexeme(st); // token + 1;
	}
	else if (ltype == NUMBERSYM)
	{
		emit(st, LIT, regtoendupin, 0, strToNum(st->cl->data));
		nextLexeme(st); // token + 1;
	}
	else
	{
		nextLexeme(st); // token + 1;
		expression(st, regtoendupin, lex_level);
		nextLexeme(st); // token + 1;
	}
	elog("} /factor()");
}

static void term(state *st, int regtoendupin, int lex_level)
{
	elog("term() {");
	factor(st, regtoendupin, lex_level);
	while (ltype == MULTSYM || ltype == SLASHSYM)
	{
		if (ltype == MULTSYM)
		{
			nextLexeme(st); // token + 1;
			factor(st, regtoendupin + 1, lex_level);
			emit(st, MUL, regtoendupin, regtoendupin, regtoendupin + 1);
		}
		if (ltype == SLASHSYM)
		{
			nextLexeme(st); // token + 1;
			factor(st, regtoendupin + 1, lex_level);
			emit(st, DIV, regtoendupin, regtoendupin, regtoendupin + 1);
		}
	}
	elog("} /term()");
}

static void expression(state *st, int regtoendupin, int lex_level)
{
	elog("expression() {");
	if (ltype == PLUSSYM)
		nextLexeme(st);
	else if (ltype == MINUSSYM)
	{
		nextLexeme(st);
		term(st, regtoendupin, lex_level);
		emit(st, NEG, regtoendupin, 0, 0);
		while (ltype == PLUSSYM || ltype == MINUSSYM)
		{
			if (ltype == PLUSSYM)
			{
				nextLexeme(st);
				term(st, regtoendupin + 1, lex_level);
				emit(st, ADD, regtoendupin, regtoendupin, regtoendupin + 1);
			}
			else
			{
				nextLexeme(st);
				term(st, regtoendupin + 1, lex_level);
				emit(st, SUB, regtoendupin, regtoendupin, regtoendupin + 1);
			}
		}
		return;
	}
	term(st, regtoendupin, lex_level);
	while (ltype == PLUSSYM || ltype == MINUSSYM)
	{
		if (ltype == PLUSSYM)
		{
			nextLexeme(st); // token + 1;
			term(st, regtoendupin + 1, lex_level);
			emit(st, ADD, regtoendupin, regtoendupin, regtoendupin + 1);
		}
		else
		{
			nextLexeme(st); // token + 1;
			term(st, regtoendupin + 1, lex_level);
			emit(st, SUB, regtoendupin, regtoendupin, regtoendupin + 1);
		}
	}
	elog("} /expression()");
}

static void condition(state *st, int lex_level)
{
	elog("condition() {");
	if (ltype == ODDSYM)
	{
		nextLexeme(st); // token + 1;
		expression(st, 0, lex_level);
		emit(st, ODD, 0, 0, 0);
	}
	else
	{
		expression(st, 0, lex_level);
		if (ltype == EQLSYM)
		{
			nextLexeme(st); // token + 1;
			expression(st, 1, lex_level);
			emit(st, EQL, 0, 0, 1);
		}
		else if (ltype == NEQSYM)
		{
			nextLexeme(st); // token + 1;
			expression(st, 1, lex_level);
			emit(st, NEQ, 0, 0, 1);
		}
		else if (ltype == LESSYM)
		{
			nextLexeme(st); // token + 1;
			expression(st, 1, lex_level);
			emit(st, LSS, 0, 0, 1);
		}
		else if (ltype == LEQSYM)
		{
			nextLexeme(st); // token + 1;
			expression(st, 1, lex_level);
			emit(st, LEQ, 0, 0, 1);
		}
		else if (ltype == GTRSYM)
		{
			nextLexeme(st); // token + 1;
			expression(st, 1, lex_level);
			emit(st, GTR, 0, 0, 1);
		}
		else if (ltype == GEQSYM)
		{
			nextLexeme(st); // token + 1;
			expression(st, 1, lex_level);
			emit(st, GEQ, 0, 0, 1);
		}
	}
    elog("} /condition()");
}

static void statement(state *st, int lex_level)
{
	elog("statement() {");
	symbol* savedSymbol;
	int savedCodeIndex;
	int savedCodeIndexForCondition;
	int savedCodeIndexForJump;
	if (ltype == IDENTSYM)
	{
		savedSymbol = symbolTableGetByNameAndClosestLevel(st->sym_table, ldata, lex_level); // WE DO NOT CHECK THAT IT'S A VARIABLE
		nextLexeme(st);
		nextLexeme(st);
		expression(st, 0, lex_level);
		emit(st, STO, 0, lex_level - savedSymbol->level, savedSymbol->addr);
	}
	else if (ltype == CALLSYM) {
		nextLexeme(st);
		savedSymbol = symbolTableGetByNameAndClosestLevel(st->sym_table, ldata, lex_level);
		emit(st, CAL, 0, lex_level - savedSymbol->level, savedSymbol->addr);
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
		elog("Begin statement closed");
		nextLexeme(st);
	}
	else if (ltype == IFSYM)
	{
		nextLexeme(st);
		condition(st, lex_level);
		int savedCodeIndexForJPC = st->ci;
		emit(st, JPC, 0, 0, 0);
		nextLexeme(st);
		statement(st, lex_level);
		if (ltype == ELSESYM) {
			nextLexeme(st);
			savedCodeIndexForJump = st->ci;
			emit(st, JMP, 0, 0, 0);
			st->code[savedCodeIndexForJPC]->m = st->ci;
			statement(st, lex_level);
			st->code[savedCodeIndexForJump]->m = st->ci;
		}
		else
			st->code[savedCodeIndexForJPC]->m = st->ci;
	}
	else if (ltype == WHILESYM)
	{
		nextLexeme(st);
		savedCodeIndexForCondition = st->ci;
		condition(st, lex_level);
		nextLexeme(st);
		savedCodeIndexForJump = st->ci;
		emit(st, JPC, 0, 0, 0);
		statement(st, lex_level);
		emit(st, JMP, 0, 0, savedCodeIndexForCondition);
		st->code[savedCodeIndexForJump]->m = st->ci;
	}
	else if (ltype == READSYM)
	{
		nextLexeme(st);
		symbol *var = symbolTableGetByNameAndClosestLevel(st->sym_table, ldata, lex_level); // NOT GUARANTEED TO BE A VAR
		nextLexeme(st);
		emit(st, SIO, 0, 0, 2); // READ
		emit(st, STO, 0, lex_level - var->level, var->addr);
	}
	else if (ltype == WRITESYM)
	{
		nextLexeme(st);
		expression(st, 0, lex_level);
		nextLexeme(st);
		emit(st, SIO, 0, 0, 1);
	}
	elog("} /statement()");
}


static int procedureDeclaration(state *st, int lex_level) {
	elog("procedureDeclaration() {");
	int procCount = 0;
	if (ltype == PROCSYM)
	{
		do
		{
			procCount++;
			st->sti++;
			nextLexeme(st);
			symbol *cur_proc = symbolTableGetByNameAndLevel(st->sym_table, ldata, lex_level);
			cur_proc->mark = 0;
			nextLexeme(st);
			// nextLexeme(st); // Was in noelle's notes
			block(st, lex_level + 1, cur_proc);
			emit(st, RTN, 0, 0, 0);
			nextLexeme(st);
		} while (ltype == PROCSYM);
	}
	elog("} /procedureDeclaration()");
	return procCount;
}


static int varDeclaration(state *st, int lex_level) {
	elog("varDeclaration() {");
	int varcount = 0;
	if (ltype == VARSYM)
	{
		do
		{
			varcount++;
			st->sti++;
			nextLexeme(st); // token + 2;
			symbolTableGetByNameAndLevel(st->sym_table, ldata, lex_level)->mark = 0;
			nextLexeme(st);
		} while (ltype == COMMASYM);
		nextLexeme(st); // token + 1;
	}
	elog("} /varDeclaration()");
	return varcount;
}

static int constDeclaration(state *st, int lex_level) {
	elog("constDeclaration() {");
	int constCount = 0;
	if (ltype == CONSTSYM)
	{
		do
		{
			constCount++;
			st->sti++;
			nextLexeme(st);
			symbolTableGetByNameAndLevel(st->sym_table, ldata, lex_level)->mark = 0;
			nextLexeme(st);
			nextLexeme(st);
			nextLexeme(st);
		} while (ltype == COMMASYM);
		nextLexeme(st); // token + 1;
	}
	return constCount;
	elog("} /constDeclaration()");
}

static void block(state *st, int lex_level, symbol *cur_procedure) {
	nextLexeme(st); // NECESSARY
	lexeme *temp_lex = st->cl;
	log("\nblock(%d) {\n", temp_lex->line);
	int varCount = 0;
	int symCount = 0;
	int old_sti = st->sti;
	symCount += constDeclaration(st, lex_level);
	varCount = varDeclaration(st, lex_level);
	symCount += procedureDeclaration(st, lex_level);
	symCount += varCount;
	cur_procedure->addr = st->ci;
	emit(st, INC, 0, 0, 3 + varCount);
	statement(st, lex_level);
	elog("Got back into the block.");
	log("\nsti=%d, old_sti=%d, symCount=%d\n", st->sti, old_sti, symCount);
	for (int i = old_sti + 1; i < symCount + old_sti + 1; i++) {
        symbol *sym = st->sym_table[i];
		log("\nsym=%s\n", sym->name);
        sym->mark = 1;
    }
	log("\n} block(%d)\n", temp_lex->line);
}

void prog(state *st) {
	elog("prog() {");
	int procCount = 1; // It's weird but it does need to start with 1
	symbol *s = st->sym_table[0]; // Random non-null value
	for (int i = 1; s != NULL; i++) {
		if (s->kind == SYMBOL_PROC) {
			s->val = procCount;
			emit(st, JMP, 0, 0, 0);
			procCount++;
		}
		s = st->sym_table[i];
	}
	
	// emit(st, JMP, 0, 0, 1); // TODO: Remove this OLD CODE
	block(st, 0, st->sym_table[0]);
	instruction *i;
	for (int j = 0; (i = st->code[j]) != NULL && i->op == JMP; j++) {
		i->m = symbolTableGetProcByValue(st->sym_table, j + 1)->addr;
	}
	printSymbolTable(st->sym_table);
	printAssemblyCode(st->code);
	exit(0);
	for (int i = 0; i < st->ci; i++) {
		instruction *instr = st->code[i];
		if (instr->op == CAL) {
			instr->m = symbolTableGetProcByValue(st->sym_table, instr->m)->addr;
		}
	}
	emit(st, SIO, 0, 0, 3); // HALT
	elog("} prog()");
}

instruction **generateAssemblyCode(symbol **sym_table, lexeme **lex_list)
{
	elog("generateAssemblyCode() {");
	instruction **code = malloc(MAX_LIST_SIZE * sizeof(struct instruction*));
	for (int i = 0; i < MAX_LIST_SIZE; i++) {
		code[i] = NULL;
	}
	state st = {
		.lex_list = lex_list,
		.sym_table = sym_table,
		.code = code,
		.lli = 0,
		.sti = 0,
		.ci = 0};

	prog(&st);
	elog("} /generateAssemblyCode()");
	return code;
}