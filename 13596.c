report_state(char *action,
	     regsub_T *sub,
	     nfa_state_T *state,
	     int lid,
	     nfa_pim_T *pim)
{
    int col;

    if (sub->in_use <= 0)
	col = -1;
    else if (REG_MULTI)
	col = sub->list.multi[0].start_col;
    else
	col = (int)(sub->list.line[0].start - rex.line);
    nfa_set_code(state->c);
    fprintf(log_fd, "> %s state %d to list %d. char %d: %s (start col %d)%s\n",
	    action, abs(state->id), lid, state->c, code, col,
	    pim_info(pim));
}