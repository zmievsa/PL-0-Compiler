// Made by Stanislav Zmiev

#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include "config.h"

struct CPU_Registers
{
	instruction *IR; // Instruction Register
	int SP;			 // stack Pointer
	int BP;			 // Base Pointer
	int PC;			 // Program Counter
} CPURegisters;

int registers[REGISTER_COUNT];
int stack[MAX_STACK_HEIGHT];
char *OPCODES[] = {
	"", "lit", "rtn", "lod", "sto", "cal", "inc", "jmp", "jpc", "sio", "neg", "add", "sub", "mul", "div", "odd", "mod", "eql", "neq", "lss", "leq", "gtr", "geq"};

void printOutput(int oldPC) {
    printf("%d %s %d %d %d\t\t\t%d\t%d\t%d\n", oldPC, OPCODES[CPURegisters.IR->op], CPURegisters.IR->r, CPURegisters.IR->l, CPURegisters.IR->m, CPURegisters.PC, CPURegisters.BP, CPURegisters.SP);
    printf("Registers:");
    for (int i = 0; i < REGISTER_COUNT; i++)
        printf(" %d", registers[i]);
    printf("\nStack:");
    for (int i = MAX_STACK_HEIGHT - 1, j = 1; i >= CPURegisters.SP; i--, j++) {
        printf(" %d", stack[i]);
        if ((j = (j%5)) == 0 && i != CPURegisters.SP)
            printf(" |");
    }
    printf("\n\n");
}

// Initializes all global constants and stack to their starting values
void initialize_globals()
{
	for (int i = 0; i < MAX_STACK_HEIGHT; i++)
		stack[i] = 0;
	for (int i = 0; i < REGISTER_COUNT; i++)
		registers[i] = 0;
	CPURegisters.SP = MAX_STACK_HEIGHT;
	CPURegisters.BP = CPURegisters.SP - 1;
	CPURegisters.PC = 0;
}

int base(int l, int base) // l stand for L in the instruction format
{
	int b1; // find base L levels up
	b1 = base;
	while (l > 0)
	{
		b1 = stack[b1];
		l--;
	}
	return b1;
}

int execute(int *instructionCounter)
{
	int halt = 0;
	switch (CPURegisters.IR->op)
	{
	// LIT
	case 1:
		registers[CPURegisters.IR->r] = CPURegisters.IR->m;
		break;

	// RTN
	case 2:
		CPURegisters.SP = CPURegisters.BP + 1;
		CPURegisters.BP = stack[CPURegisters.SP - 2];
		CPURegisters.PC = stack[CPURegisters.SP - 3];
		break;

	// LOD
	case 3:
		registers[CPURegisters.IR->r] = stack[base(CPURegisters.IR->l, CPURegisters.BP) - CPURegisters.IR->m];
		break;

	// STO
	case 4:
		stack[base(CPURegisters.IR->l, CPURegisters.BP) - CPURegisters.IR->m] = registers[CPURegisters.IR->r];
		break;

	// CAL
	case 5:
		stack[CPURegisters.SP - 1] = base(CPURegisters.IR->l, CPURegisters.BP); // Static Link
		stack[CPURegisters.SP - 2] = CPURegisters.BP;							// Dynamic Link
		stack[CPURegisters.SP - 3] = CPURegisters.PC;							// Return Adress
		CPURegisters.BP = CPURegisters.SP - 1;
		CPURegisters.PC = CPURegisters.IR->m;
		break;

	// INC
	case 6:
		CPURegisters.SP -= CPURegisters.IR->m;
		break;

	// JMP
	case 7:
		CPURegisters.PC = CPURegisters.IR->m;
		break;

	// JPC
	case 8:
		if (registers[CPURegisters.IR->r] == 0)
		{
			CPURegisters.PC = CPURegisters.IR->m;
		}
		break;

	// SYS
	case 9:
		if (CPURegisters.IR->m == 1)
			printf("\nRegister 0:%d\n\n", registers[CPURegisters.IR->r]);
		else if (CPURegisters.IR->m == 2)
			scanf("%d", &registers[CPURegisters.IR->r]);
		else
			halt = 1;
		break;

	// NEG
	case 10:
		registers[CPURegisters.IR->r] = -registers[CPURegisters.IR->r];
		break;

	// ADD
	case 11:
		registers[CPURegisters.IR->r] = registers[CPURegisters.IR->l] + registers[CPURegisters.IR->m];
		break;

	// SUB
	case 12:
		registers[CPURegisters.IR->r] = registers[CPURegisters.IR->l] - registers[CPURegisters.IR->m];
		break;

	// MUL
	case 13:
		registers[CPURegisters.IR->r] = registers[CPURegisters.IR->l] * registers[CPURegisters.IR->m];
		break;

	// DIV
	case 14:
		registers[CPURegisters.IR->r] = registers[CPURegisters.IR->l] / registers[CPURegisters.IR->m];
		break;

	// ODD
	case 15:
		registers[CPURegisters.IR->r] %= 2;
		break;

	// MOD
	case 16:
		registers[CPURegisters.IR->r] = registers[CPURegisters.IR->l] % registers[CPURegisters.IR->m];
		break;

	// EQL
	case 17:
		registers[CPURegisters.IR->r] = registers[CPURegisters.IR->l] == registers[CPURegisters.IR->m];
		break;

	// NEQ
	case 18:
		registers[CPURegisters.IR->r] = registers[CPURegisters.IR->l] != registers[CPURegisters.IR->m];
		break;

	// LSS
	case 19:
		registers[CPURegisters.IR->r] = registers[CPURegisters.IR->l] < registers[CPURegisters.IR->m];
		break;

	// LEQ
	case 20:
		registers[CPURegisters.IR->r] = registers[CPURegisters.IR->l] <= registers[CPURegisters.IR->m];
		break;

	// GTR
	case 21:
		registers[CPURegisters.IR->r] = registers[CPURegisters.IR->l] > registers[CPURegisters.IR->m];
		break;

	// GEQ
	case 22:
		registers[CPURegisters.IR->r] = registers[CPURegisters.IR->l] >= registers[CPURegisters.IR->m];
		break;
	}
	return halt;
}

void executeBytecode(instruction **code, int print_execution_trace)
{
	initialize_globals();
	int i = 0;
	int halt = 0;
	int *instructionCounter = (int *)malloc(sizeof(int));
	*instructionCounter = 0;
	if (print_execution_trace) {
		printf("\t\t\t\t\tpc\tbp\tsp\n");
		printf("Initial values\t\t%d\t%d\t%d\n",
			CPURegisters.PC, CPURegisters.BP, CPURegisters.SP
		);
		printf("Registers:");
		for (int i = 0; i < REGISTER_COUNT; i++)
			printf(" %d", registers[i]);
		printf("\nStack:");
		for (int i = 0; i < 20; i++)
			printf(" %d", stack[i]);
		printf("\n\n");
	}
	while (halt == 0)
	{
		// Fetch
		CPURegisters.IR = *(code + CPURegisters.PC);
		int oldPC = CPURegisters.PC;
		// Increment PC
		CPURegisters.PC++;

		// Execute
		halt = execute(instructionCounter);
		if (print_execution_trace)
			printOutput(oldPC);
		if (CPURegisters.IR->op != 7)
			(*instructionCounter)++;
	}
	free(instructionCounter);
}
