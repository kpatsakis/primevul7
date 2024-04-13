nfa_dump(nfa_regprog_T *prog)
{
    FILE *debugf = fopen(NFA_REGEXP_DUMP_LOG, "a");

    if (debugf != NULL)
    {
	nfa_print_state(debugf, prog->start);

	if (prog->reganch)
	    fprintf(debugf, "reganch: %d\n", prog->reganch);
	if (prog->regstart != NUL)
	    fprintf(debugf, "regstart: %c (decimal: %d)\n",
					      prog->regstart, prog->regstart);
	if (prog->match_text != NULL)
	    fprintf(debugf, "match_text: \"%s\"\n", prog->match_text);

	fclose(debugf);
    }
}