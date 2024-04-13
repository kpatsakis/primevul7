spell_load_cb(char_u *fname, void *cookie)
{
    spelload_T	*slp = (spelload_T *)cookie;
    slang_T	*slang;

    slang = spell_load_file(fname, slp->sl_lang, NULL, FALSE);
    if (slang != NULL)
    {
	// When a previously loaded file has NOBREAK also use it for the
	// ".add" files.
	if (slp->sl_nobreak && slang->sl_add)
	    slang->sl_nobreak = TRUE;
	else if (slang->sl_nobreak)
	    slp->sl_nobreak = TRUE;

	slp->sl_slang = slang;
    }
}