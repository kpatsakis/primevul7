nfa_did_time_out()
{
    if (nfa_time_limit != NULL && profile_passed_limit(nfa_time_limit))
    {
	if (nfa_timed_out != NULL)
	    *nfa_timed_out = TRUE;
	return TRUE;
    }
    return FALSE;
}