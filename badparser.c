#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"
#include "codegen.h"

int symTablePos = 0, tokenTablePos = 0;

// Various
int MCodePos = 0, currentM = 0;
int varLevel = 0, constLevel = 0, varNum = 0, constNum = 0;

#define ttype st->cl->type
#define nextLexeme() st->cl = st->lex_list[(st->li)++]
// Gets variable/const name from the next lexeme (assuming we're on the VAR or CONST lexeme rn)
#define getVarName() st->lex_list[st->li + 1]->data

// Gets variable/const value from the next lexeme (assuming we're on the VAR or CONST lexeme rn)
#define getVarValue() symbolTableGet(st->sym_table, getVarName())->val

typedef struct state
{
    int cur_level;
    int varcount;
    int proc_count;
    char *proc;
    symbol **sym_table;
    int si; // Next sym_table index
    lexeme **lex_list;
    int li; // Next lex_list index
    // lexeme *cur_lex;
    instruction **code;
    int ci;     // Next code index
    lexeme *cl; // Current lex
} state;

void analyze(lexeme **lex_list)
{
    symbol *symbol_table[MAX_LIST_SIZE] = {NULL};
    instruction *code[MAX_LIST_SIZE] = {NULL};
    state states = {
        .varcount = 0,
        .cur_level = 0,
        .proc_count = 0,
        .lex_list = lex_list,
        .sym_table = symbol_table,
        .code = code,
        .si = 0,
        .li = 0};
    state *st = &states;
    block(st);
    if (ttype != PERIODSYM)
    { //"."

        error(24, st->cl);
    }
}
//the real bulkiness
void block(state *st)
{
    int tempBlockPos = MCodePos, temPos;
    currentM = 0;

    emit(st, 7, 0, 0, 0); //Start.

    while (ttype == CONSTSYM || ttype == VARSYM)
    {
        if (ttype == CONSTSYM)
            constant(st);
        if (ttype == VARSYM)
            variable(st);
        st->varcount++;
    }

    temPos = currentM; //Store current MCode pos

    while (ttype == PROCSYM)
        procedure(st);

    st->code[tempBlockPos]->m = MCodePos;

    emit(st, 6, 0, 0, temPos + 4);
    statement(st);
    if (ttype != PERIODSYM && ttype == SEMICOLONSYM)
    {
        emit(st, 2, 0, 0, 0); //return proc
        voidSyms(st->cur_level);
    }
    else
        emit(st, 9, 0, 0, 2);
}
//deal with consts
void constant(state *st)
{
    char lex_name[MAX_IDENTIFIER_LEN];
    int returner = 0;
    if (constNum >= 1)
    {
        if (constLevel == st->cur_level)
        {

            error(25);
        }
    }
    constNum++;
    constLevel = st->cur_level;
    do
    {
        nextLexeme();
        if (ttype != IDENTSYM)
        {

            error(8); //const/int/proc must have ident after
        }
        returner = symbolTableGet(st->cl->data, st->cur_level);
        if (returner != -1 && st->sym_table[returner]->level == st->cur_level)
        {

            error(26); //semicolon needed between statements
        }

        strcpy(lex_name, st->cl->data); //copy into temp

        nextLexeme();
        if (ttype != EQLSYM)
        {

            error(4); //equals wanted after const declaration
        }

        nextLexeme();
        if (ttype != NUMBERSYM)
        {

            error(5); //number wanted after equals in const
        }

        pushSymTable(1, lex_name, constLevel, -5, toInt(st->cl->data));
        nextLexeme();
    } while (ttype == COMMASYM);

    if (ttype != SEMICOLONSYM)
    {

        error(13); //semicolon needed between statements
    }

    nextLexeme();
}
//Deal with vars
void variable(state *st)
{
    int returner = 0;
    if (varNum >= 1)
    {
        if (varLevel == st->cur_level)
        {

            error(25);
        }
    }
    varNum++;
    varLevel = st->cur_level;

    do
    {
        nextLexeme();
        if (ttype != IDENTSYM)
        {

            error(8); //const/int/proc must have ident after
        }             //Test to see if it exists already

        returner = searchSym(st->cl->data, st->cur_level);
        if (returner != -1 && st->sym_table[returner]->level == st->cur_level)
        {

            error(26); //semicolon needed between statements
        }
        pushSymTable(2, currentToken, st->cur_level, currentM + 4, 0);
        nextLexeme();
    } while (ttype == COMMASYM);

    if (ttype != SEMICOLONSYM)
    {

        error(13); //semicolon needed between statements
    }

    nextLexeme();
}
//Deal with a statement line
void statement(state *st)
{
    int symPos, identPos, tempBPos, temPos, temPos2;

    if (ttype == IDENTSYM)
    {
        symPos = searchSym(st->cl->data, st->cur_level);

        if (symPos == -1)
        {

            printf("Identifier '%s': ", st->cl->data);
            error(15); //undeclared variable found
        }
        else if (st->sym_table[symPos]->kind == 1)
        {

            error(12); //assignment to const/proc not valid
        }

        identPos = st->sym_table[symPos]->addr;

        nextLexeme();
        if (ttype != BECOMESSYM)
        {
            if (ttype == EQLSYM)
            {

                error(3); //use := not =
            }
            else
            {

                error(9); // := expected
            }
        }

        nextLexeme();
        expression(st);

        if (ttype != SEMICOLONSYM)
        {

            error(13); //semicolon needed between statements
        }

        emit(st, 4, 0, st->cur_level - st->sym_table[symPos]->level, identPos);
    }
    else if (ttype == CALLSYM)
    {
        nextLexeme();

        if (ttype != IDENTSYM)
        {

            error(10); //assignment to const/proc not valid
        }

        symPos = searchSym(st->cl->data, st->cur_level);

        if (symPos == -1)
        {

            printf("Identifier '%s': ", st->cl->data);
            error(15); //undeclared variable found
        }
        else if (st->sym_table[symPos]->kind == 1)
        {

            error(12); //assignment to const/proc not valid
        }

        nextLexeme();

        emit(st, 5, 0, st->cur_level, st->sym_table[symPos]->addr);
    }
    else if (ttype == BEGINSYM)
    {
        nextLexeme();
        statement(st);

        while (ttype == SEMICOLONSYM)
        {
            nextLexeme();
            statement(st);
        }
        if (ttype != ENDSYM)
        {

            error(14); //ENDSYM expected
        }

        nextLexeme();
    }
    else if (ttype == IFSYM)
    {
        nextLexeme();
        operation(st);
        if (ttype != THENSYM)
        {

            error(6); // then expected after if
        }

        nextLexeme();
        tempBPos = MCodePos;

        emit(st, 8, 0, 0, 0);

        statement(st);
        st->code[tempBPos]->m = MCodePos;

        nextLexeme();

        if (ttype != ELSESYM)
        {
            tokenTablePos--;
            tokenTablePos--;
            ttype = st->lex_list[tokenTablePos]->type;
            strcpy(st->cl->data, st->lex_list[tokenTablePos]->data);
            while (ttype == NEWLI)
            {
                tokenTablePos--;
                ttype = st->lex_list[tokenTablePos]->type;
                strcpy(st->cl->data, st->lex_list[tokenTablePos]->data);
            }
        }

        if (ttype == ELSESYM)
        {
            st->code[tempBPos]->m = MCodePos + 1;

            tempBPos = MCodePos;

            emit(st, 7, 0, 0, 0);

            nextLexeme();
            statement(st);
            st->code[tempBPos]->m = MCodePos;
        }
    }
    else if (ttype == WHILESYM)
    {
        temPos = MCodePos;

        nextLexeme();
        operation(st);

        temPos2 = MCodePos;

        emit(st, 8, 0, 0, 0);

        if (ttype != DOSYM)
        {

            error(7); //do expected after while
        }

        nextLexeme();
        statement(st);

        emit(st, 7, 0, 0, temPos);

        st->code[temPos2]->m = MCodePos;
    }
    else if (ttype == READSYM)
    {
        nextLexeme();

        if (ttype == IDENTSYM)
        {
            symPos = searchSym(st->cl->data, st->cur_level);
            if (symPos == -1)
            {

                printf("Identifier '%s': ", st->cl->data);
                error(15); //undeclared variable found
            }
            nextLexeme();

            emit(st, 9, 0, 0, 1); //read from screen

            emit(st, 4, 0, 0, st->sym_table[symPos]->addr); //increment mcode
        }
    }
    else if (ttype == WRITESYM)
    {
        nextLexeme();

        if (ttype == IDENTSYM)
        {
            symPos = searchSym(st->cl->data, st->cur_level);
            if (symPos == -1)
            {

                printf("Identifier '%s': ", st->cl->data);
                error(15); //undeclared variable found
            }
            nextLexeme();
            if (st->sym_table[symPos]->kind == 1)
                emit(st, 1, 0, 0, st->sym_table[symPos]->val); //if constant
            else
                emit(st, 3, 0, 0, st->sym_table[symPos]->addr); //read from screen

            emit(st, 9, 0, 0, 0); //output
        }
        else
        {

            error(10); //undeclared variable found
        }
    }
}
//Expression work
void expressionFound(state *st)
{
    int thisOp;

    if (ttype == PLUSSYM || ttype == MINUSSYM)
    {
        thisOp = ttype;

        if (thisOp == MINUSSYM)
        {
            nextLexeme();
            term(st);
            emit(st, 2, 0, 0, 1);
        }
    }
    else
    {
        term(st);
    }

    while (ttype == PLUSSYM || ttype == MINUSSYM)
    {
        thisOp = ttype;
        nextLexeme();
        term(st);

        if (thisOp == PLUSSYM)
            emit(st, 2, 0, 0, 2);
        else
            emit(st, 2, 0, 0, 3);
    }
}
//terms and things
void termFound(state *st)
{
    int thisOp;

    factor(st);

    while (ttype == MULTSYM || ttype == SLASHSYM)
    {
        thisOp = ttype;
        nextLexeme();
        factor(st);

        if (thisOp == MULTSYM)
            emit(st, 2, 0, 0, 4);
        else
            emit(st, 2, 0, 0, 5);
    }
}
//factors and fun
void factorFound(state *st)
{
    int symPos;

    if (ttype == IDENTSYM)
    {
        symPos = searchSym(st->cl->data, st->cur_level);

        if (symPos == -1)
        {

            printf("Identifier '%s': ", st->cl->data);
            error(15); //undeclared variable found
        }

        if (st->sym_table[symPos]->kind == 1)
            emit(st, 1, 0, 0, st->sym_table[symPos]->val);
        else
            emit(st, 3, 0, st->cur_level - st->sym_table[symPos]->level, st->sym_table[symPos]->addr);

        nextLexeme();
    }
    else if (ttype == NUMBERSYM)
    {
        emit(st, 1, 0, 0, toInt(st->cl->data));
        nextLexeme();
    }
    else if (ttype == LPARENTSYM)
    {
        nextLexeme();
        expression(st);

        if (ttype != RPARENTSYM)
        {

            error(16); //error: ) missing
        }

        nextLexeme();
    }
    else
    {

        error(14); //cannot begin with this symbol
    }
}
//Mathematical operation
void operationFound(state *st)
{
    int thisOp;
    if (ttype == ODDSYM)
    {
        emit(st, 2, 0, 0, 6);
        nextLexeme();
        expression(st);
    }
    else
    {
        expression(st);
        thisOp = ttype;

        switch (ttype)
        {
        case BECOMESSYM:

            error(2);
            break;
        case EQLSYM:
            thisOp = 8;
            break;

        case NEQSYM:
            thisOp = 9;
            break;

        case LESSYM:
            thisOp = 10;
            break;

        case LEQSYM:
            thisOp = 11;
            break;

        case GTRSYM:
            thisOp = 12;
            break;

        case GEQSYM:
            thisOp = 13;
            break;

        default:

            error(11); //relational op needed.
            break;
        }
        nextLexeme();
        expression(st);
        emit(st, 2, 0, 0, thisOp);
    }
}
//deal with procedure declarations
void procedureFound(state *st)
{
    st->proc_count++;
    nextLexeme();
    if (ttype != IDENTSYM)
    {

        error(8); //const/int/proc must have ident after
    }
    strcpy(currentProc, st->cl->data);

    pushSymTable(3, currentToken, st->cur_level, MCodePos, -1);

    st->cur_level++;
    if (st->cur_level > 10)
    {
        error(23);
    }
    st->proc_count++;
    varNum = 0;

    nextLexeme();
    if (ttype != SEMICOLONSYM)
    {

        error(13); //semicolon needed between statements
    }

    nextLexeme();
    block(); //run for the proc's insides
    st->cur_level--;

    if (ttype != SEMICOLONSYM)
    {

        error(13); //semicolon needed between statements
    }
    strcpy(currentProc, " ");
    nextLexeme();
}
//find a variable in the symbol table
int searchSym(char *name, int level)
{
    int i;
    while (level != -1)
    {
        for (i = symTablePos - 1; i >= 0; i--)
        {
            if ((strcmp(name, st->sym_table[i].name) == 0) && (st->sym_table[i].addr != -1) && (st->sym_table[i].level == level))
            {
                return i;
            }
        }
        level--;
    }
    return -1; //not found :(
}
//mark old syms as invalid with -1
void voidSyms(int level)
{
    int i;
    for (i = symTablePos - 1; i >= 0; i--)
    {
        if (st->sym_table[i].level == level && st->sym_table[i].kind != 3 && st->sym_table[i].addr != -1)
        {
            st->sym_table[i].addr = -1;
        }
    }
}
//Add to sym table
void pushSymTable(int kind, Token t, int L, int M, int num)
{
    st->sym_table[symTablePos].kind = kind;
    strcpy(st->sym_table[symTablePos].name, t.name);
    st->sym_table[symTablePos].level = L;
    st->sym_table[symTablePos].addr = M;
    if (kind == 1)
        st->sym_table[symTablePos].val = num;
    else if (kind == 2)
        currentM++;
    else if (kind == 3)
    {
        procedures[procPos][0] = M;
        procedures[procPos][1] = L + 1;
        procPos++;
    }
    symTablePos++;
}
//Print the MCode to the screen if needed
void printMCode(int flag)
{
    char c;
    fileMCode = fopen(nameMCode, "r");
    if (fileMCode == NULL)
        error(1);
    if (flag)
    {
        printf("========================\nGenerated Machine Code\n========================\n");
        c = fgetc(fileMCode);
        while (c != EOF)
        {
            printf("%c", c);
            c = fgetc(fileMCode);
        }
        printf("\n\n");
    }
    fclose(fileMCode);
}

int toInt(char *num)
{
    int returner = 0, i = 0;
    while (num[i] != '\0')
    {
        returner *= 10;
        returner += num[i] - '0';
        i++;
    }
    return returner;
}

//push the code
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