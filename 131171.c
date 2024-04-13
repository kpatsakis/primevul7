spell_soundfold(
    slang_T	*slang,
    char_u	*inword,
    int		folded,	    // "inword" is already case-folded
    char_u	*res)
{
    char_u	fword[MAXWLEN];
    char_u	*word;

    if (slang->sl_sofo)
	// SOFOFROM and SOFOTO used
	spell_soundfold_sofo(slang, inword, res);
    else
    {
	// SAL items used.  Requires the word to be case-folded.
	if (folded)
	    word = inword;
	else
	{
	    (void)spell_casefold(curwin,
				  inword, (int)STRLEN(inword), fword, MAXWLEN);
	    word = fword;
	}

	if (has_mbyte)
	    spell_soundfold_wsal(slang, word, res);
	else
	    spell_soundfold_sal(slang, word, res);
    }
}