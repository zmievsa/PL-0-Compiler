// SYMBOL TABLE, LEXEME LIST, AND INPUT FILE ARE AT MOST 500 ELEMENTS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexeme.h"
#include "scanner.h"
#include "symbol.h"
#include "vm.h"
#include "config.h"
#include "parser.h"
#include "codegen.h"


int main(int argc, char *argv[])
{
    int print_lexeme_list = 0, print_assembly_code = 0, print_execution_trace = 0;
    char *input_filename = 0;
    for (int i = 1; i < argc; i++)
    {
        if (streql(argv[i], "-l"))
            print_lexeme_list = 1;
        else if (streql(argv[i], "-a"))
            print_assembly_code = 1;
        else if (streql(argv[i], "-v"))
            print_execution_trace = 1;
        else
            input_filename = argv[i];
    }
    if (input_filename == 0)
    {
        printf("Error: please, include the file name.");
        return 1;
    }
    int success = printFileContents(input_filename);
    if (!success) {
        printf("\nFile '%s' does not exist.\n", input_filename);
        exit(1);
    }

    lexeme **lexemes = buildLexemeTable(input_filename);
    if (lexemes[0] == NULL) {
        printf("\nNo lexemes found\n");
        exit(1);
    }
    if (print_lexeme_list)
    {
        printLexemeTable(lexemes);
        printRawLexemeList(lexemes);
        printFormattedLexemeList(lexemes);
    }

    symbol **symbol_table = buildSymbolTable(lexemes);
    printf("\nNo errors, program is syntactically correct.\n");
    printSymbolTable(symbol_table);
    instruction **assembly_code = generateAssemblyCode(symbol_table, lexemes);
    if (print_assembly_code)
        printAssemblyCode(assembly_code);
    executeBytecode(assembly_code, print_execution_trace);
}