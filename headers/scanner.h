#ifndef __SCANNER_H
#define __SCANNER_H
#include "lexeme.h"

int printFileContents(char *filename);
lexeme **buildLexemeTable(char *input_file);

#endif