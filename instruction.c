#include "instruction.h"
#include <stdio.h>
#include "config.h"


char *OPNAMES[23] = {NULL, "LIT", "RTN", "LOD", "STO", "CAL", "INC", "JMP", "JPC", "SIO", "NEG", "ADD", "SUB", "MUL", "DIV", "ODD", "MOD", "EQL", "NEQ", "LSS", "LEQ", "GTR", "GEQ"};

char* opname(int op) {
    return OPNAMES[op];
}


void printAssemblyCode(instruction** code) {
    elog("printAssemblyCode()");
    printf("\n\nAssembly Code:\nLine\tOP\tR\tL\tM\n");
    for (int i = 0; i < MAX_LIST_SIZE; i++) {
        instruction *instr = code[i];
        if (instr == NULL)
            break; 
        printf("%d\t\t%s\t%d\t%d\t%d\n", i, instr->op_name, instr->r, instr->l, instr->m);
    }
    elog("/printAssemblyCode()");
    printf("\n\n");
}