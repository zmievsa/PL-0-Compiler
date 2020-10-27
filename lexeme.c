#ifndef LEXEME_FILE
#define LEXEME_FILE

#include <stdlib.h>
#include "lexeme.h"

struct lexeme *LEXEME_LL_TAIL = NULL;
// Pointer for iteration
struct lexeme *LEXEME_LL_ITER_CURRENT = NULL;
struct lexeme *LEXEME_LL_HEAD = NULL;

//insert link at the last location
void lexeme_insert(char *data, int type)
{
    struct lexeme *link = (struct lexeme *)malloc(sizeof(struct lexeme));

    link->data = data;
    link->type = type;
    link->next = NULL;

    if (LEXEME_LL_HEAD == NULL)
    {
        LEXEME_LL_TAIL = link;
        LEXEME_LL_HEAD = link;
        LEXEME_LL_ITER_CURRENT = link;
    }
    else
    {
        LEXEME_LL_HEAD->next = link;
        LEXEME_LL_HEAD = link;
    }
}

// Reset iteration to be able to iterate linked list again
void lexeme_reset_iter()
{
    LEXEME_LL_ITER_CURRENT = LEXEME_LL_TAIL;
}

// Return next item in iteration
// Returns NULL if iteration is over
struct lexeme *lexeme_iter()
{
    //save reference to first link
    struct lexeme *tempLink = LEXEME_LL_ITER_CURRENT;
    if (LEXEME_LL_ITER_CURRENT != NULL)
        LEXEME_LL_ITER_CURRENT = LEXEME_LL_ITER_CURRENT->next;

    if (tempLink == NULL)
        lexeme_reset_iter();
    return tempLink;
}

#endif