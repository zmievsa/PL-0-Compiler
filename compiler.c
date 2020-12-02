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
// #include "codegen.h"

// This function might be entirely unnecessary
// int strarr_contains(char *str_arr[], int str_arr_len, char *str)
// {
//     for (int i = 0; i < str_arr_len; i++)
//         if (streql(str_arr[i], str))
//             return 1;
//     return 0;
// }

// This function might be entirely unnecessary
// char *readFile(char *filename)
// {
//     FILE *ifp = fopen(filename, "r");
//     char *inputfile = malloc(500 * sizeof(char));
//     char c = fgetc(ifp);
//     int i = 0;
//     while (1)
//     {
//         inputfile[i++] = c;
//         c = fgetc(ifp);
//         if (c == EOF)
//             break;
//     }
//     inputfile[i] = '\0';
//     return inputfile;
// }

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

    lexeme **lexemes = buildLexemeTable(input_filename);
    if (print_lexeme_list)
    {
        printRawLexemeList(lexemes);
        printFormattedLexemeList(lexemes);
    }

    symbol **symbol_table = buildSymbolTable(lexemes);
    printf("\nNo errors, program is syntactically correct.\n");
    printSymbolTable(symbol_table);
    // instruction **assembly_code = generateAssemblyCode(symbol_table, lexemes);
    // if (print_assembly_code)
    //     printAssemblyCode(assembly_code);
}