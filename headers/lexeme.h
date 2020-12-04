#ifndef __LEXEME_H
#define __LEXEME_H
#include "config.h"

// LEXEME TYPES
#define NULSYM 1
#define IDENTSYM 2
#define NUMBERSYM 3
#define PLUSSYM 4
#define MINUSSYM 5
#define MULTSYM 6
#define SLASHSYM 7
#define ODDSYM 8
#define EQLSYM 9
#define NEQSYM 10
#define LESSYM 11
#define LEQSYM 12
#define GTRSYM 13
#define GEQSYM 14
#define LPARENTSYM 15
#define RPARENTSYM 16
#define COMMASYM 17
#define SEMICOLONSYM 18
#define PERIODSYM 19
#define BECOMESSYM 20
#define BEGINSYM 21
#define ENDSYM 22
#define IFSYM 23
#define THENSYM 24
#define WHILESYM 25
#define DOSYM 26
#define CALLSYM 27
#define CONSTSYM 28
#define VARSYM 29
#define PROCSYM 30
#define WRITESYM 31
#define READSYM 32
#define ELSESYM 33

typedef struct lexeme
{
    char *data;
    int type;
    int line;
} lexeme;

void error(char *msg, lexeme *lex);
lexeme *scanLexeme(char *token, int token_len, int line);
void printLexemeTable(lexeme **lexemes);
void printRawLexemeList(lexeme *lexemes[]);
void printFormattedLexemeList(lexeme *lexemes[]);

#endif