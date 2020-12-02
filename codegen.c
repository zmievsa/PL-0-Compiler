#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"
#include "codegen.h"

#define ttype st->cl->type
#define nextLexeme() st->cl = st->lex_list[(st->li)++]
// Gets variable/const name from the next lexeme (assuming we're on the VAR or CONST lexeme rn)
#define getVarName() st->lex_list[st->li + 1]->data

// Gets variable/const value from the next lexeme (assuming we're on the VAR or CONST lexeme rn)
#define getVarValue() symbolTableGet(st->sym_table, getVarName())->val

typedef struct state
{
	symbol **sym_table;
	int si; // Next sym_table index
	lexeme **lex_list;
	int li; // Next lex_list index
	// lexeme *cur_lex;
	instruction **code;
	int ci;		// Next code index
	lexeme *cl; // Current lex
} state;

void emit(state *st, int op, int r, int l, int m)
{
	if (st->ci > MAX_LIST_SIZE)
		error("too much code");
	else
	{
		instruction *instr = st->code[st->ci];
		instr->op = op;
		instr->r = r;
		instr->l = l;
		instr->m = m;
		st->ci++;
	}
}
int strToNum(char *str)
{
	static char *endptr;
	return (int)strtol(str, &endptr, 10);
}
void factor(state *st, int regtoendupin)
{
	symbol *sym;
	int t = st->cl;
	if (t == IDENTSYM)
	{
		sym = st->sym_table[st->si];
		if (t == CONSTSYM)
			emit(st, LIT, regtoendupin, 0, sym->val);
		if (ttype == VARSYM)
			emit(st, LOD, regtoendupin, 0, sym->val);
		nextLexeme(); // token + 1;
	}
	else if (t == NUMBERSYM)
	{
		emit(st, LIT, regtoendupin, 0, strToNum(st->cl->data));
		nextLexeme(); // token + 1;
	}
	else
	{
		nextLexeme(); // token + 1;
		EXPRESSION(regtoendupin);
		nextLexeme(); // token + 1;
	}
}

void term(state *st, int regtoendupin)
{
	FACTOR(regtoendupin);
	while (ttype == MULTSYM || ttype == SLASHSYM)
	{
		if (ttype == MULTSYM)
		{
			nextLexeme(); // token + 1;
			FACTOR(regtoendupin + 1);
			emit(st, MUL, regtoendupin, regtoendupin, regtoendupin + 1);
		}
		if (ttype == SLASHSYM)
		{
			nextLexeme(); // token + 1;
			FACTOR(regtoendupin + 1);
			emit(st, DIV, regtoendupin, regtoendupin, regtoendupin + 1);
		}
	}
}

void expression(state *st, int regtoendupin)
{
	if (ttype == PLUSSYM)
		nextLexeme(); // token + 1;
	if (ttype == MINUSSYM)
	{
		nextLexeme(); // token + 1;
		TERM(regtoendupin);
		emit(st, NEG, regtoendupin, 0, 0);
		while (ttype == PLUSSYM || ttype == MINUSSYM)
		{
			if (ttype == PLUSSYM)
			{
				nextLexeme(); // token + 1;
				TERM(regtoendupin + 1);
				emit(st, ADD, regtoendupin, regtoendupin, regtoendupin + 1);
			}
			if (ttype == MINUSSYM)
			{
				nextLexeme(); // token + 1;
				TERM(regtoendupin + 1);
				emit(st, SUB, regtoendupin, regtoendupin, regtoendupin + 1);
			}
		}
		return;
	}
	TERM(regtoendupin);
	while (ttype == PLUSSYM || ttype == MINUSSYM)
	{
		if (ttype == PLUSSYM)
		{
			nextLexeme(); // token + 1;
			TERM(regtoendupin + 1);
			emit(st, ADD, regtoendupin, regtoendupin, regtoendupin + 1);
		}
		if (ttype == MINUSSYM)
		{
			nextLexeme(); // token + 1;
			TERM(regtoendupin + 1);
			emit(st, SUB, regtoendupin, regtoendupin, regtoendupin + 1);
		}
	}
}

void condition(state *st)
{
	if (ttype == ODDSYM)
	{
		nextLexeme(); // token + 1;
		EXPRESSION(0);
		emit(st, ODD, 0, 0, 0);
	}
	else
	{
		EXPRESSION(0);
		if (ttype == EQLSYM)
		{
			nextLexeme(); // token + 1;
			EXPRESSION(1);
			emit(st, EQL, 0, 0, 1);
		}
		if (ttype == NEQSYM)
		{
			nextLexeme(); // token + 1;
			EXPRESSION(1);
			emit(st, NEQ, 0, 0, 1);
		}
		if (ttype == LESSYM)
		{
			nextLexeme(); // token + 1;
			EXPRESSION(1);
			emit(st, LSS, 0, 0, 1);
		}
		if (ttype == LEQSYM)
		{
			nextLexeme(); // token + 1;
			EXPRESSION(1);
			emit(st, LEQ, 0, 0, 1);
		}
		if (ttype == GTRSYM)
		{
			nextLexeme(); // token + 1;
			EXPRESSION(1);
			emit(st, GTR, 0, 0, 1);
		}
		if (ttype == GEQSYM)
		{
			nextLexeme(); // token + 1;
			EXPRESSION(1);
			emit(st, GEQ, 0, 0, 1);
		}
	}
}

statement(state *st)
{
	int savedSymbolTableIndex;
	instruction *savedCode;
	int savedCodeIndexForCondition;
	instruction *savedCodeForJump;
	if (ttype == IDENTSYM)
	{
		savedSymbolTableIndex = st->si;
		nextLexeme(); // token + 2;
		EXPRESSION(0);
		emit(st, STO, 0, 0, st->sym_table[savedSymbolTableIndex]->val);
	}
	if (ttype == BEGINSYM)
	{
		nextLexeme(); // token + 1;
		statement(st);
		while (ttype == SEMICOLONSYM)
		{
			nextLexeme(); // token + 1;
			statement(st);
		}
		nextLexeme(); // token + 1;
	}
	if (ttype == IFSYM)
	{
		nextLexeme(); // token + 1;
		condition(st);
		savedCode = st->code[st->ci];
		emit(st, JPC, 0, 0, 0);
		nextLexeme(); // token + 1;
		statement(st);
		savedCode->m = st->code[st->ci]->m;
	}
	if (ttype == WHILESYM)
	{
		nextLexeme(); // token + 1;
		savedCodeIndexForCondition = st->ci;
		condition(st);
		nextLexeme(); // token + 1;
		savedCodeForJump = st->code[st->ci];
		emit(st, JPC, 0, 0, 0);
		statement(st);
		emit(st, JMP, 0, 0, savedCodeIndexForCondition);
		savedCodeForJump->m = st->ci;
	}
	if (ttype == READSYM)
	{
		nextLexeme(); // token + 1;
		// ? save the symbol table index;
		nextLexeme();			// token + 1;
		emit(st, SIO, 0, 0, 2); // READ
		emit(st, STO, 0, 0, getVarValue());
	}
	if (ttype == WRITESYM)
	{
		nextLexeme(); // token + 1;
		// ? save the symbol table index;
		if (ttype == VARSYM)
		{
			emit(st, LOD, 0, 0, getVarValue()); // M comes from the symbol table
			emit(st, SIO, 0, 0, 1);				// WRITE
		}
		if (ttype == CONSTSYM)
		{
			emit(st, LIT, 0, 0, getVarValue()); // M comes from the symbol table(val))
			emit(st, SIO, 0, 0, 1);				// WRITE
		}
		nextLexeme(); // token + 1;
	}
}

void block(state *st)
{
	int varcount = 0;
	st->cl = st->lex_list[0];
	if (ttype == CONSTSYM)
	{
		do
		{
			nextLexeme(); // token + 4;
		} while (ttype == COMMASYM);
		nextLexeme(); // token + 1;
	}
	if (ttype == VARSYM)
	{
		do
		{
			varcount++;
			nextLexeme(); // token + 2;
		} while (ttype == COMMASYM);
		nextLexeme(); // token + 1;
	}
	emit(st, INC, 0, 0, 3 + varcount);
	statement(st);
}

instruction **generateAssemblyCode(symbol **sym_table, lexeme **lex_list)
{
	instruction *code[MAX_LIST_SIZE] = {NULL};
	state st = {
		.lex_list = lex_list,
		.sym_table = sym_table,
		.code = code,
		.li = 0,
		.si = 0,
		.ci = 0};

	emit(&st, JMP, 0, 0, 1);
	block(&st);
	emit(&st, SIO, 0, 0, 3);
	printf("code has been generated\n");
	return code;
}