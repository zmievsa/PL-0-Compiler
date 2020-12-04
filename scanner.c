// TODO: Check if <> (NEQLSYM) sequence is handled correctly

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "scanner.h"
#include "lexeme.h"
#include "config.h"
#include "errors.h"


#define SCANNER_ONLY_DEBUG 0
#define isInvis strcontains(INVISIBLE_CHARS, lookahead_c)
#define isSpec strcontains(SPECIAL_SYMBOLS, lookahead_c)
#define applyingOP (is_special && (isLatinAlpha(lookahead_c) || isdigit((int)lookahead_c)))

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
int check_token(char *token, int token_len, int size_overflow, int invalid_symbols_present, int line, int is_comment)
{
    lexeme unfinishedLex = {
        .data =  token,
        .type = -1,
        .line = line
    };
    if (invalid_symbols_present && !is_comment)
    {
        error(1, "Invalid symbol present.", &unfinishedLex);
    }
    else if (isdigit(token[0]) && !is_comment)
    {
        for (int i = 0; i < token_len; i++)
        {
            if (isLatinAlpha(token[i]))
                error(2, "Variable does not start with a letter.", &unfinishedLex);
        }
        if (size_overflow || token_len > MAX_NUM_LEN)
            error(3, "Number is too long.", &unfinishedLex);
    }
    else if (isLatinAlpha(token[0]) && size_overflow && !is_comment)
        error(4, "Variable name is too long.", &unfinishedLex);
    else if (token_len > 1)
    {
        if (streql(token, "/*"))
            return COMMENT_START;
        else if (streql(token, "*/"))
            if (is_comment)
                return COMMENT_END;
            else
                error(5, "Unexpected comment end symbol.", &unfinishedLex);
    }
    return 0;
}

// Returns 0 on no errors
lexeme **buildLexemeTable(char *filename)
{
    lexeme **lexemes = malloc(MAX_LIST_SIZE * sizeof(struct lexeme *));
    for (int i = 0; i < MAX_LIST_SIZE; i++)
        lexemes[i] = NULL;
    int lexeme_list_len = 0;

    FILE *fp = fopen(filename, "r");
    char *token = malloc(MAX_IDENTIFIER_LEN * sizeof(char) + 1);
    reset_str(token, MAX_IDENTIFIER_LEN + 1);
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
    int line = 1;
    while ((lookahead_c = fgetc(fp)))
    {
        spec = 0;
        invis = 0;
        applying_op = 0;
        invis = isInvis;
        spec = isSpec;
        applying_op = applyingOP;
        if (lookahead_c == EOF || (invis || (spec || applying_op)) && token_len != 0)
        {
            if (!applying_op && spec && is_special && token_len == 1 && (token[0] == '<' || token[0] == '>' || token[0] == ':' || (token[0] == '/' && lookahead_c == '*') || (token[0] == '*' && lookahead_c == '/')))
                ; // Let the compound operator finish building up
            // Error checking
            else if (check_error_code = check_token(token, token_len, size_overflow, invalid_symbols_present, line, is_comment))
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
                    exit(1);
            }
            else if (token_len != 0)
            {
                lexemes[lexeme_list_len++] = scanLexeme(token, token_len, line);
                token = malloc(MAX_IDENTIFIER_LEN + 1);
                reset_str(token, token_len);
                token_len = 0;
                size_overflow = 0;
                is_special = 0;
            }
            if (lookahead_c == EOF)
            {
                free(token);
                return lexemes;
            }
        }
        if (lookahead_c == '\n')
            line++;
        if (is_comment && !((token_len == 0 && lookahead_c == '*') || (token_len == 1 && lookahead_c == '/')))
            continue;
        if (spec)
            is_special = 1;
        else if (invis)
            continue;
        if (!size_overflow && !invalid_symbols_present) {
            token[token_len++] = lookahead_c;
        }
        if (token_len > MAX_IDENTIFIER_LEN)
            size_overflow = 1;
        if (!isdigit(lookahead_c) && !isLatinAlpha(lookahead_c) && !strcontains(SPECIAL_SYMBOLS, lookahead_c))
            invalid_symbols_present = 1;
    }
}

int printFileContents(char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
        return 0;
    printf("Input file:\n");
    char c;
    while ((c = fgetc(fp)) != EOF)
        printf("%c", c);
    printf("\n\n");
    fclose(fp);
    return 1;
}
