#ifndef __INSTRUCTION_H
#define __INSTRUCTION_H

#define LIT 1
#define RTN 2
#define LOD 3
#define STO 4
#define CAL 5
#define INC 6
#define JMP 7
#define JPC 8
#define SIO 9
#define NEG 10
#define ADD 11
#define SUB 12
#define MUL 13
#define DIV 14
#define ODD 15
#define MOD 16
#define EQL 17
#define NEQ 18
#define LSS 19
#define LEQ 20
#define GTR 21
#define GEQ 22

char *opname(int op);

typedef struct instruction
{
    int op; // opcode
    char *op_name;
    int r; // R
    int l; // L
    int m; // M
} instruction;

void printAssemblyCode(instruction** code);


#endif