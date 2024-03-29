#ifndef __CONFIG_H
#define __CONFIG_H
#include <string.h>
#include <stdio.h>

#define MAX_LIST_SIZE 500
#define MAX_IDENTIFIER_LEN 11
#define MAX_NUM_LEN 5
#define streql(A, S) (strcmp(A, S) == 0)
#define LOGGING_ENABLED 0
#if LOGGING_ENABLED
#define elog(x) printf("\n" x "\n")
#define log(x, ...) printf(x, __VA_ARGS__)
#else
#define elog(x) ;
#define log(x, ...) ;
#endif

#endif