#ifndef __CODEGEN_H
#define __CODEGEN_H

#include "instruction.h"
#include "symbol.h"
#include "lexeme.h"

instruction **generateAssemblyCode(symbol **sym_table, lexeme **lex_list);

#endif