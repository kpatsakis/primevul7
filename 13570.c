nfa_regfree(regprog_T *prog)
{
    if (prog != NULL)
    {
	vim_free(((nfa_regprog_T *)prog)->match_text);
	vim_free(((nfa_regprog_T *)prog)->pattern);
	vim_free(prog);
    }
}