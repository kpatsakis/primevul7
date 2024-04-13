STATIC unsigned  int
S_regex_set_precedence(const U8 my_operator) {

    /* Returns the precedence in the (?[...]) construct of the input operator,
     * specified by its character representation.  The precedence follows
     * general Perl rules, but it extends this so that ')' and ']' have (low)
     * precedence even though they aren't really operators */

    switch (my_operator) {
        case '!':
            return 5;
        case '&':
            return 4;
        case '^':
        case '|':
        case '+':
        case '-':
            return 3;
        case ')':
            return 2;
        case ']':
            return 1;
    }

    NOT_REACHED; /* NOTREACHED */
    return 0;   /* Silence compiler warning */