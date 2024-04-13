nfa_restore_listids(nfa_regprog_T *prog, int *list)
{
    int		    i;
    nfa_state_T	    *p;

    p = &prog->state[0];
    for (i = prog->nstate; --i >= 0; )
    {
	p->lastlist[1] = list[i];
	++p;
    }
}