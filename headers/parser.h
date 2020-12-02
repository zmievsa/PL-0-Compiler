#ifndef __PARSER_H
#define __PARSER_H

#include "symbol.h"
#include "lexeme.h"

symbol **buildSymbolTable(lexeme **lex_list);

#endif