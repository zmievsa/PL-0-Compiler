#ifndef __VM_H
#define __VM_H
#include "instruction.h"

#define MAX_STACK_HEIGHT 1000
#define REGISTER_COUNT 8

void executeBytecode(instruction **code, int print_execution_trace);

#endif