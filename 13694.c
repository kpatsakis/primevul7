ex_setfiletype(exarg_T *eap)
{
    if (!did_filetype)
    {
	char_u *arg = eap->arg;

	if (STRNCMP(arg, "FALLBACK ", 9) == 0)
	    arg += 9;

	set_option_value_give_err((char_u *)"filetype", 0L, arg, OPT_LOCAL);
	if (arg != eap->arg)
	    did_filetype = FALSE;
    }
}