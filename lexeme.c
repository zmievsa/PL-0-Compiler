#include <stdlib.h>
#include "lexeme.h"
#include "config.h"
#include <stdio.h>
#include <ctype.h>

char *LEXEME_TYPES[] = {
    "",
    "nulsym",
    "identsym",
    "numbersym",
    "plussym",
    "minussym",
    "multsym",
    "slashsym",
    "oddsym",
    "eqlsym",
    "neqsym",
    "lessym",
    "leqsym",
    "gtrsym",
    "geqsym",
    "lparentsym",
    "rparentsym",
    "commasym",
    "semicolonsym",
    "periodsym",
    "becomessym",
    "beginsym",
    "endsym",
    "ifsym",
    "thensym",
    "whilesym",
    "dosym",
    "callsym",
    "constsym",
    "varsym",
    "procsym",
    "writesym",
    "readsym",
    "elsesym",
};

void error(char *msg, lexeme *lex) {
    printf("\n\nError on line %d on token '%s': %s\n", lex->line, lex->data, msg);
    exit(1);
}

lexeme *scanLexeme(char *token, int token_len, int line)
{
    int type = -1;
    if (token_len == 1)
    {
        switch (token[0])
        {
        case ('+'):
            type = PLUSSYM;
            break;

        case ('-'):
            type = MINUSSYM;
            break;

        case ('*'):
            type = MULTSYM;
            break;

        case ('/'):
            type = SLASHSYM;
            break;

        case ('('):
            type = LPARENTSYM;
            break;

        case (')'):
            type = RPARENTSYM;
            break;

        case ('='):
            type = EQLSYM;
            break;

        case (','):
            type = COMMASYM;
            break;

        case ('.'):
            type = PERIODSYM;
            break;

        case ('<'):
            type = LESSYM;
            break;

        case ('>'):
            type = GTRSYM;
            break;

        case (';'):
            type = SEMICOLONSYM;
            break;
        }
    }
    else
    {
        if (streql(token, "const"))
            type = CONSTSYM;
        else if (streql(token, "var"))
            type = VARSYM;
        else if (streql(token, "procedure"))
            type = PROCSYM;
        else if (streql(token, "call"))
            type = CALLSYM;
        else if (streql(token, "begin"))
            type = BEGINSYM;
        else if (streql(token, "end"))
            type = ENDSYM;
        else if (streql(token, "if"))
            type = IFSYM;
        else if (streql(token, "then"))
            type = THENSYM;
        else if (streql(token, "else"))
            type = ELSESYM;
        else if (streql(token, "while"))
            type = WHILESYM;
        else if (streql(token, "do"))
            type = DOSYM;
        else if (streql(token, "read"))
            type = READSYM;
        else if (streql(token, "write"))
            type = WRITESYM;
        else if (streql(token, "odd"))
            type = ODDSYM;
        else if (streql(token, ":="))
            type = BECOMESSYM;
        else if (streql(token, "<>"))
            type = NEQSYM;
        else if (streql(token, ">="))
            type = GEQSYM;
        else if (streql(token, "<="))
            type = LEQSYM;
    }
    if (type == -1)
    {
        if (isdigit(token[0]))
            type = NUMBERSYM;
        else
            type = IDENTSYM;
    }
    lexeme *lex = malloc(sizeof(lexeme));

    lex->data = token;
    lex->type = type;
    lex->line = line;
    return lex;
}

void printLexemeTable(lexeme **lexemes) {
    printf("Lexeme Table:\nlexeme\t\ttoken type\n");
    lexeme *lex;
    for (int i = 0; i < MAX_LIST_SIZE; i++)
    {
        lex = lexemes[i];
        if (lex == NULL)
            break;
        printf("%s\t\t%d\n", lex->data, lex->type);
    }
    printf("\n\n");
}

void printRawLexemeList(lexeme *lexemes[])
{
    elog("printRawLexemeList()");
    printf("Raw Lexeme List:\n");
    lexeme *lex;
    for (int i = 0; i < MAX_LIST_SIZE; i++)
    {
        lex = lexemes[i];
        if (lex == NULL)
            break;
        printf("%d ", lex->type);
        if (lex->type == 2 || lex->type == 3)
            printf("%s ", lex->data);
    }
    printf("\n\n");
}

void printFormattedLexemeList(lexeme *lexemes[])
{
    elog("printFormattedLexemeList()");
    printf("Formatted Lexeme List:\n");
    lexeme *lex;
    for (int i = 0; i < MAX_LIST_SIZE; i++)
    {
        lex = lexemes[i];
        if (lex == NULL)
            break;
        printf("%s ", LEXEME_TYPES[lex->type]);
        if (lex->type == 2 || lex->type == 3)
            printf("%s ", lex->data);
    }
    printf("\n\n");
}
