nfa_postfix_dump(char_u *expr, int retval)
{
    int *p;
    FILE *f;

    f = fopen(NFA_REGEXP_DUMP_LOG, "a");
    if (f != NULL)
    {
	fprintf(f, "\n-------------------------\n");
	if (retval == FAIL)
	    fprintf(f, ">>> NFA engine failed... \n");
	else if (retval == OK)
	    fprintf(f, ">>> NFA engine succeeded !\n");
	fprintf(f, "Regexp: \"%s\"\nPostfix notation (char): \"", expr);
	for (p = post_start; *p && p < post_ptr; p++)
	{
	    nfa_set_code(*p);
	    fprintf(f, "%s, ", code);
	}
	fprintf(f, "\"\nPostfix notation (int): ");
	for (p = post_start; *p && p < post_ptr; p++)
		fprintf(f, "%d ", *p);
	fprintf(f, "\n\n");
	fclose(f);
    }
}