regdump(char_u *pattern, bt_regprog_T *r)
{
    char_u  *s;
    int	    op = EXACTLY;	// Arbitrary non-END op.
    char_u  *next;
    char_u  *end = NULL;
    FILE    *f;

#ifdef BT_REGEXP_LOG
    f = fopen("bt_regexp_log.log", "a");
#else
    f = stdout;
#endif
    if (f == NULL)
	return;
    fprintf(f, "-------------------------------------\n\r\nregcomp(%s):\r\n", pattern);

    s = r->program + 1;
    // Loop until we find the END that isn't before a referred next (an END
    // can also appear in a NOMATCH operand).
    while (op != END || s <= end)
    {
	op = OP(s);
	fprintf(f, "%2d%s", (int)(s - r->program), regprop(s)); // Where, what.
	next = regnext(s);
	if (next == NULL)	// Next ptr.
	    fprintf(f, "(0)");
	else
	    fprintf(f, "(%d)", (int)((s - r->program) + (next - s)));
	if (end < next)
	    end = next;
	if (op == BRACE_LIMITS)
	{
	    // Two ints
	    fprintf(f, " minval %ld, maxval %ld", OPERAND_MIN(s), OPERAND_MAX(s));
	    s += 8;
	}
	else if (op == BEHIND || op == NOBEHIND)
	{
	    // one int
	    fprintf(f, " count %ld", OPERAND_MIN(s));
	    s += 4;
	}
	else if (op == RE_LNUM || op == RE_COL || op == RE_VCOL)
	{
	    // one int plus comparator
	    fprintf(f, " count %ld", OPERAND_MIN(s));
	    s += 5;
	}
	s += 3;
	if (op == ANYOF || op == ANYOF + ADD_NL
		|| op == ANYBUT || op == ANYBUT + ADD_NL
		|| op == EXACTLY)
	{
	    // Literal string, where present.
	    fprintf(f, "\nxxxxxxxxx\n");
	    while (*s != NUL)
		fprintf(f, "%c", *s++);
	    fprintf(f, "\nxxxxxxxxx\n");
	    s++;
	}
	fprintf(f, "\r\n");
    }

    // Header fields of interest.
    if (r->regstart != NUL)
	fprintf(f, "start `%s' 0x%x; ", r->regstart < 256
		? (char *)transchar(r->regstart)
		: "multibyte", r->regstart);
    if (r->reganch)
	fprintf(f, "anchored; ");
    if (r->regmust != NULL)
	fprintf(f, "must have \"%s\"", r->regmust);
    fprintf(f, "\r\n");

#ifdef BT_REGEXP_LOG
    fclose(f);
#endif
}