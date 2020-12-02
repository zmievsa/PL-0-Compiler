#ifndef __ERRORS_H
#define __ERRORS_H

// ERROR TYPES
#define VAR_NOT_START_WITH_LETTER 1
#define NUM_TOO_LONG 2
#define NAME_TOO_LONG 3
#define INV_SYMB_PRESENT 4
#define COMMENT_START 5
#define COMMENT_END 6

char *errors[] = {
    "",
    "The variable does not start with a letter.",
    "The number is too long.",
    "The name is too long.",
    "Invisible symbols present."};

#endif