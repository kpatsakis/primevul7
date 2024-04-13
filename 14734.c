do_finish(exarg_T *eap, int reanimate)
{
    int		idx;

    if (reanimate)
	((source_cookie_T *)getline_cookie(eap->getline,
					      eap->cookie))->finished = FALSE;

    // Cleanup (and inactivate) conditionals, but stop when a try conditional
    // not in its finally clause (which then is to be executed next) is found.
    // In this case, make the ":finish" pending for execution at the ":endtry".
    // Otherwise, finish normally.
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    if (idx >= 0)
    {
	eap->cstack->cs_pending[idx] = CSTP_FINISH;
	report_make_pending(CSTP_FINISH, NULL);
    }
    else
	((source_cookie_T *)getline_cookie(eap->getline,
					       eap->cookie))->finished = TRUE;
}