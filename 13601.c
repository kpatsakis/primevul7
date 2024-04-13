clear_sub(regsub_T *sub)
{
    if (REG_MULTI)
	// Use 0xff to set lnum to -1
	vim_memset(sub->list.multi, 0xff,
				  sizeof(struct multipos) * rex.nfa_nsubexpr);
    else
	vim_memset(sub->list.line, 0,
				   sizeof(struct linepos) * rex.nfa_nsubexpr);
    sub->in_use = 0;
}