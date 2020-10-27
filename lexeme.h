struct lexeme
{
    char *data;
    int type;
    struct lexeme *next;
};

void lexeme_insert(char *data, int type);
struct lexeme *lexeme_iter();
