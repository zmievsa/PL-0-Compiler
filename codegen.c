#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"
#include "codegen.h"
#include "config.h"

#define ltype st->cl->type
// Gets variable/const name from the next lexeme (assuming we're one past the VAR or CONST lexeme rn)
#define getVarName() st->cl->data

// Gets variable/const value from the next lexeme (assuming we're on the VAR or CONST lexeme rn)
#define curVar symbolTableGet(st->sym_table, getVarName())

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

static void factor(state *st, int regtoendupin);
static void term(state *st, int regtoendupin);
static void expression(state *st, int regtoendupin);
static void condition(state *st);
static void statement(state *st);
static void varDeclaration(state *st, int *varcount);
static void constDeclaration(state *st);
static void block(state *st);

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
		log("\nemit: %s\t%d\t%d\t%d\n", instr->op_name, r, l, m);
	}
}
static int strToNum(char *str)
{
	static char *endptr;
	return (int)strtol(str, &endptr, 10);
}
static void factor(state *st, int regtoendupin)
{
	elog("factor() {");
	symbol *sym;
	if (ltype == IDENTSYM)
	{
		log("\nfound identsym '%s'\n", st->cl->data);
		sym = curVar;
		if (sym->kind == SYMBOL_CONST)
			emit(st, LIT, regtoendupin, 0, sym->val);
		else if (sym->kind == SYMBOL_VAR)
			emit(st, LOD, regtoendupin, 0, sym->addr);
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
		expression(st, regtoendupin);
		nextLexeme(st); // token + 1;
	}
	elog("} /factor()");
}

static void term(state *st, int regtoendupin)
{
	elog("term() {");
	factor(st, regtoendupin);
	while (ltype == MULTSYM || ltype == SLASHSYM)
	{
		if (ltype == MULTSYM)
		{
			elog("began multiplication");
			nextLexeme(st); // token + 1;
			factor(st, regtoendupin + 1);
			emit(st, MUL, regtoendupin, regtoendupin, regtoendupin + 1);
		}
		if (ltype == SLASHSYM)
		{
			nextLexeme(st); // token + 1;
			factor(st, regtoendupin + 1);
			emit(st, DIV, regtoendupin, regtoendupin, regtoendupin + 1);
		}
	}
	elog("} /term()");
}

static void expression(state *st, int regtoendupin)
{
	elog("expression() {");
	if (ltype == PLUSSYM)
		nextLexeme(st); // token + 1;
	else if (ltype == MINUSSYM)
	{
		// for (int i = st->lli - 6; i < st->lli + 5; i++)
		// 	log("\nCur: %s\n", st->lex_list[i]->data);
		nextLexeme(st); // token + 1;
		term(st, regtoendupin);
		emit(st, NEG, regtoendupin, 0, 0);
		while (ltype == PLUSSYM || ltype == MINUSSYM)
		{
			if (ltype == PLUSSYM)
			{
				nextLexeme(st); // token + 1;
				term(st, regtoendupin + 1);
				emit(st, ADD, regtoendupin, regtoendupin, regtoendupin + 1);
			}
			else
			{
				nextLexeme(st); // token + 1;
				term(st, regtoendupin + 1);
				emit(st, SUB, regtoendupin, regtoendupin, regtoendupin + 1);
			}
		}
		return;
	}
	term(st, regtoendupin);
	while (ltype == PLUSSYM || ltype == MINUSSYM)
	{
		if (ltype == PLUSSYM)
		{
			nextLexeme(st); // token + 1;
			term(st, regtoendupin + 1);
			emit(st, ADD, regtoendupin, regtoendupin, regtoendupin + 1);
		}
		else
		{
			nextLexeme(st); // token + 1;
			term(st, regtoendupin + 1);
			emit(st, SUB, regtoendupin, regtoendupin, regtoendupin + 1);
		}
	}
	elog("} /expression()");
}

static void condition(state *st)
{
	elog("condition() {");
	if (ltype == ODDSYM)
	{
		nextLexeme(st); // token + 1;
		expression(st, 0);
		emit(st, ODD, 0, 0, 0);
	}
	else
	{
		expression(st, 0);
		if (ltype == EQLSYM)
		{
			nextLexeme(st); // token + 1;
			expression(st, 1);
			emit(st, EQL, 0, 0, 1);
		}
		else if (ltype == NEQSYM)
		{
			nextLexeme(st); // token + 1;
			expression(st, 1);
			emit(st, NEQ, 0, 0, 1);
		}
		else if (ltype == LESSYM)
		{
			nextLexeme(st); // token + 1;
			expression(st, 1);
			emit(st, LSS, 0, 0, 1);
		}
		else if (ltype == LEQSYM)
		{
			nextLexeme(st); // token + 1;
			expression(st, 1);
			emit(st, LEQ, 0, 0, 1);
		}
		else if (ltype == GTRSYM)
		{
			nextLexeme(st); // token + 1;
			expression(st, 1);
			emit(st, GTR, 0, 0, 1);
		}
		else if (ltype == GEQSYM)
		{
			nextLexeme(st); // token + 1;
			expression(st, 1);
			emit(st, GEQ, 0, 0, 1);
		}
	}
    elog("} /condition()");
}

static void statement(state *st)
{
	elog("statement() {");
	symbol* savedSymbol;
	int savedCodeIndex;
	int savedCodeIndexForCondition;
	int savedCodeIndexForJump;
	if (ltype == IDENTSYM)
	{
		savedSymbol = curVar;
		nextLexeme(st); // token + 2;
		nextLexeme(st);
		expression(st, 0);
		emit(st, STO, 0, 0, savedSymbol->addr);
	}
	else if (ltype == BEGINSYM)
	{
		nextLexeme(st); // token + 1;
		statement(st);
		while (ltype == SEMICOLONSYM)
		{
			nextLexeme(st); // token + 1;
			statement(st);
		}
		nextLexeme(st); // token + 1;
	}
	else if (ltype == IFSYM)
	{
		nextLexeme(st); // token + 1;
		condition(st);
		savedCodeIndex = st->ci;
		emit(st, JPC, 0, 0, 0);
		nextLexeme(st); // token + 1;
		statement(st);
		st->code[savedCodeIndex]->m = st->ci;
	}
	else if (ltype == WHILESYM)
	{
		nextLexeme(st); // token + 1;
		savedCodeIndexForCondition = st->ci;
		condition(st);
		nextLexeme(st); // token + 1;
		savedCodeIndexForJump = st->ci;
		emit(st, JPC, 0, 0, 0);
		statement(st);
		emit(st, JMP, 0, 0, savedCodeIndexForCondition);
		st->code[savedCodeIndexForJump]->m = st->ci;
	}
	else if (ltype == READSYM)
	{
		nextLexeme(st); // token + 1;
		symbol *var = curVar;
		log("\nvar=%p\n", var);
		nextLexeme(st);			// token + 1;
		emit(st, SIO, 0, 0, 2); // READ
		emit(st, STO, 0, 0, var->addr); // Not sure if address is the correct attr
	}
	else if (ltype == WRITESYM)
	{
		elog("Found WRITESYM");
		nextLexeme(st); // token + 1;
		symbol *sym = curVar; // Can also be a const
		if (ltype == SYMBOL_VAR)
		{
			emit(st, LOD, 0, 0, sym->addr); // M comes from the symbol table
			emit(st, SIO, 0, 0, 1);				// WRITE
		}
		else if (ltype == SYMBOL_VAR)
		{
			emit(st, LIT, 0, 0, sym->val); // M comes from the symbol table(val))
			emit(st, SIO, 0, 0, 1);				// WRITE
		}
		nextLexeme(st); // token + 1;
	}
	elog("} /statement()");
}

static void varDeclaration(state *st, int *varcount) {
	elog("varDeclaration() {");
	if (ltype == VARSYM)
	{
		do
		{
			(*varcount)++;
			nextLexeme(st); // token + 2;
			nextLexeme(st);
		} while (ltype == COMMASYM);
		nextLexeme(st); // token + 1;
	}
	elog("} /varDeclaration()");
}

static void constDeclaration(state *st) {
	elog("constDeclaration() {");
	if (ltype == CONSTSYM)
	{
		do
		{
			nextLexeme(st); // token + 4;
			nextLexeme(st);
			nextLexeme(st);
			nextLexeme(st);
		} while (ltype == COMMASYM);
		nextLexeme(st); // token + 1;
	}
	elog("} /constDeclaration()");
}

static void block(state *st)
{
	elog("block() {");
	int varcount = 0;
	nextLexeme(st); // NECESSARY
	constDeclaration(st);
	varDeclaration(st, &varcount);
	emit(st, INC, 0, 0, 3 + varcount);
	statement(st);
	elog("} /block()");
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

	emit(&st, JMP, 0, 0, 1);
	block(&st);
	emit(&st, SIO, 0, 0, 3);
	elog("} /generateAssemblyCode()");
	return code;
}