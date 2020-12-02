#ifndef __CONFIG_H
#define __CONFIG_H
#include <string.h>

#define MAX_LIST_SIZE 500
#define MAX_IDENTIFIER_LEN 11
#define MAX_NUM_LEN 5
#define streql(A, S) (strcmp(A, S) == 0)
// #define log(x, ...) printf(x, __VA_ARGS__)
#define log(x, ...) ;

#endif