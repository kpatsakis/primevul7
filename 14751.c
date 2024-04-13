ex_scriptversion(exarg_T *eap UNUSED)
{
    int		nr;

    if (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptversion_used_outside_of_sourced_file));
	return;
    }
    if (in_vim9script())
    {
	emsg(_(e_cannot_use_scriptversion_after_vim9script));
	return;
    }

    nr = getdigits(&eap->arg);
    if (nr == 0 || *eap->arg != NUL)
	emsg(_(e_invalid_argument));
    else if (nr > SCRIPT_VERSION_MAX)
	semsg(_(e_scriptversion_not_supported_nr), nr);
    else
    {
	current_sctx.sc_version = nr;
#ifdef FEAT_EVAL
	SCRIPT_ITEM(current_sctx.sc_sid)->sn_version = nr;
#endif
    }
}