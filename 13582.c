st_error(int *postfix UNUSED, int *end UNUSED, int *p UNUSED)
{
#ifdef NFA_REGEXP_ERROR_LOG
    FILE *df;
    int *p2;

    df = fopen(NFA_REGEXP_ERROR_LOG, "a");
    if (df)
    {
	fprintf(df, "Error popping the stack!\n");
# ifdef DEBUG
	fprintf(df, "Current regexp is \"%s\"\n", nfa_regengine.expr);
# endif
	fprintf(df, "Postfix form is: ");
# ifdef DEBUG
	for (p2 = postfix; p2 < end; p2++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
	nfa_set_code(*p);
	fprintf(df, "\nCurrent position is: ");
	for (p2 = postfix; p2 <= p; p2 ++)
	{
	    nfa_set_code(*p2);
	    fprintf(df, "%s, ", code);
	}
# else
	for (p2 = postfix; p2 < end; p2++)
	    fprintf(df, "%d, ", *p2);
	fprintf(df, "\nCurrent position is: ");
	for (p2 = postfix; p2 <= p; p2 ++)
	    fprintf(df, "%d, ", *p2);
# endif
	fprintf(df, "\n--------------------------\n");
	fclose(df);
    }
#endif
    emsg(_("E874: (NFA) Could not pop the stack!"));
}