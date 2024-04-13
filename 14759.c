ex_packadd(exarg_T *eap)
{
    static char *plugpat = "pack/*/%s/%s";
    int		len;
    char	*pat;
    int		round;
    int		res = OK;

    // Round 1: use "start", round 2: use "opt".
    for (round = 1; round <= 2; ++round)
    {
	// Only look under "start" when loading packages wasn't done yet.
	if (round == 1 && did_source_packages)
	    continue;

	len = (int)STRLEN(plugpat) + (int)STRLEN(eap->arg) + 5;
	pat = alloc(len);
	if (pat == NULL)
	    return;
	vim_snprintf(pat, len, plugpat, round == 1 ? "start" : "opt", eap->arg);
	// The first round don't give a "not found" error, in the second round
	// only when nothing was found in the first round.
	res = do_in_path(p_pp, (char_u *)pat,
		DIP_ALL + DIP_DIR + (round == 2 && res == FAIL ? DIP_ERR : 0),
		add_pack_plugin, eap->forceit ? &APP_ADD_DIR : &APP_BOTH);
	vim_free(pat);
    }
}