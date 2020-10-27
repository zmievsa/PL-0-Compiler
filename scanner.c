#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexeme.h"

int buildLexemeTable(char *filename);

#define SCANNER_ONLY_DEBUG 1

// MAX LENGTHS
#define MAX_LEX_LEN MAX_IDENTIFIER_LEN
#define MAX_IDENTIFIER_LEN 11
#define MAX_NUM_LEN 5

// ERROR TYPES
#define VAR_NOT_START_WITH_LETTER 1
#define NUM_TOO_LONG 2
#define NAME_TOO_LONG 3
#define INV_SYMB_PRESENT 4
#define COMMENT_START 5
#define COMMENT_END 6

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

const char *RESERVED_KEYWORDS[] = {"const", "var", "procedure", "call", "begin", "end", "if", "then", "else", "while", "do", "read", "write", "odd"};
const char SPECIAL_SYMBOLS[] = "+-*/()=,.<>;:";
const char INVISIBLE_CHARS[] = "\t \n";

int isLatinAlpha(char c)
{
    return (c >= 65 && c <= 90) || (c >= 97 && c <= 122);
}

int strcontains(const char str[], char c)
{
    return (strchr(str, c) != NULL);
}

void reset_str(char *str, int len)
{
    for (int i = 0; i < len; i++)
        str[i] = '\0';
}

// Nonzero value means error
int check_token(char *token, int token_len, int size_overflow, int invalid_symbols_present)
{
    if (invalid_symbols_present)
    {
        printf("\nInvalid symbol present error.\n");
        return INV_SYMB_PRESENT;
    }
    else if (isdigit(token[0]))
    {
        for (int i = 0; i < token_len; i++)
        {
            if (isLatinAlpha(token[i]))
            {
                printf("\nVariable does not start with a letter error.\n");
                return VAR_NOT_START_WITH_LETTER;
            }
        }
        if (size_overflow || token_len > MAX_NUM_LEN)
        {
            printf("\nNumber is too long error.\n");
            return NUM_TOO_LONG;
        }
    }
    else if (isLatinAlpha(token[0]) && size_overflow)
    {
        printf("\nVariable name is too long error.\n");
        return NAME_TOO_LONG;
    }
    else if (token_len > 1)
    {
        if (!strcmp(token, "/*"))
            return COMMENT_START;
        else if (!strcmp(token, "*/"))
            return COMMENT_END;
    }
    return 0;
}

void saveToken(char *token, int token_len)
{
    int type = -1;
    if (token_len == 1)
    {
        switch (token[0])
        {
        case ('+'):
            type = 4;
            break;

        case ('-'):
            type = 5;
            break;

        case ('*'):
            type = 6;
            break;

        case ('/'):
            type = 7;
            break;

        case ('('):
            type = 15;
            break;

        case (')'):
            type = 16;
            break;

        case ('='):
            type = 9;
            break;

        case (','):
            type = 17;
            break;

        case ('.'):
            type = 19;
            break;

        case ('<'):
            type = 11;
            break;

        case ('>'):
            type = 13;
            break;

        case (';'):
            type = 18;
            break;
        }
    }
    else
    {
        if (!strcmp(token, "const"))
            type = CONSTSYM;
        else if (!strcmp(token, "var"))
            type = VARSYM;
        else if (!strcmp(token, "procedure"))
            type = PROCSYM;
        else if (!strcmp(token, "call"))
            type = CALLSYM;
        else if (!strcmp(token, "begin"))
            type = BEGINSYM;
        else if (!strcmp(token, "end"))
            type = ENDSYM;
        else if (!strcmp(token, "if"))
            type = IFSYM;
        else if (!strcmp(token, "then"))
            type = THENSYM;
        else if (!strcmp(token, "else"))
            type = ELSESYM;
        else if (!strcmp(token, "while"))
            type = WHILESYM;
        else if (!strcmp(token, "do"))
            type = DOSYM;
        else if (!strcmp(token, "read"))
            type = READSYM;
        else if (!strcmp(token, "write"))
            type = WRITESYM;
        else if (!strcmp(token, "odd"))
            type = ODDSYM;
        else if (!strcmp(token, ":="))
            type = BECOMESSYM;
    }
    if (type == -1)
    {
        if (isdigit(token[0]))
            type = NUMBERSYM;
        else
            type = IDENTSYM;
    }
    lexeme_insert(token, type);
}

// Returns 0 on no errors
int buildLexemeTable(char *filename)
{
    FILE *fp = fopen(filename, "r");
    char *token = malloc(MAX_LEX_LEN * sizeof(char) + 1);
    reset_str(token, MAX_LEX_LEN + 1);
    int token_len = 0;
    char lookahead_c;
    int size_overflow = 0;
    int invalid_symbols_present = 0;
    int comment_beginning = 0;
    int encountered_star_in_comment = 0;
    int is_comment = 0;
    int is_special = 0;
    int applying_op = 0;
    int invis = 0;
    int spec = 0;
    int check_error_code = 0;
    while ((lookahead_c = fgetc(fp)))
    {
        spec = 0;
        invis = 0;
        applying_op = 0;
        if (lookahead_c == EOF || (((invis = strcontains(INVISIBLE_CHARS, lookahead_c) || (spec = strcontains(SPECIAL_SYMBOLS, lookahead_c) || (applying_op = (is_special && (isLatinAlpha(lookahead_c) || isdigit(lookahead_c)))))))) && token_len != 0)
        {
            if (!applying_op && spec && is_special && token_len == 1)
                ; // Let the compound operator finish building up
            // Error checking
            else if (check_error_code = check_token(token, token_len, size_overflow, invalid_symbols_present))
            {
                reset_str(token, token_len);
                token_len = 0;
                invalid_symbols_present = 0;
                size_overflow = 0;
                is_special = 0;
                if (check_error_code == COMMENT_START)
                    is_comment = 1;
                else if (check_error_code == COMMENT_END)
                    is_comment = 0;
                else
                    return check_error_code;
            }
            else
            {
                saveToken(token, token_len);
                token = malloc(MAX_LEX_LEN + 1);
                reset_str(token, token_len);
                token_len = 0;
                size_overflow = 0;
                is_special = 0;
            }
            if (lookahead_c == EOF)
            {
                free(token);
                return 0;
            }
        }
        if (is_comment && !((token_len == 0 && lookahead_c == '*') || (token_len == 1 && lookahead_c == '/')))
            continue;
        if (spec)
            is_special = 1;
        else if (invis)
            continue;
        if (!size_overflow && !invalid_symbols_present)
            token[token_len++] = lookahead_c;
        if (token_len > MAX_LEX_LEN)
            size_overflow = 1;
        if (!isdigit(lookahead_c) && !isLatinAlpha(lookahead_c) && !strcontains(SPECIAL_SYMBOLS, lookahead_c))
            invalid_symbols_present = 1;
    }
}

#if SCANNER_ONLY_DEBUG == 1

void printFileContents(char *filename)
{
    printf("Source Program:\n");
    FILE *fp = fopen(filename, "r");
    char c;
    while ((c = fgetc(fp)) != EOF)
        printf("%c", c);
    printf("\n");
    fclose(fp);
}

void printLexemeTable()
{
    printf("Lexeme Table:\nlexeme\ttoken type\n");
    struct lexeme *current_lexeme;
    while ((current_lexeme = lexeme_iter()) != NULL)
    {
        printf("%s\t%d\n", current_lexeme->data, current_lexeme->type);
    }
    printf("\n");
}

void printLexemeList()
{
    printf("Lexeme List:\n");
    struct lexeme *current_lexeme;
    while ((current_lexeme = lexeme_iter()) != NULL)
    {
        printf("%d ", current_lexeme->type);
        if (current_lexeme->type == 2 || current_lexeme->type == 3)
            printf("%s ", current_lexeme->data);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Not enough args.\n");
        return 1;
    }
    char *filename = argv[1];
    printFileContents(filename);
    int err_code = buildLexemeTable(filename);
    if (err_code)
        return 1;
    printLexemeTable();
    printLexemeList();
}

#endif
