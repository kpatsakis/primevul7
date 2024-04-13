spell_delete_wordlist(void)
{
    char_u	fname[MAXPATHL];

    if (int_wordlist != NULL)
    {
	mch_remove(int_wordlist);
	int_wordlist_spl(fname);
	mch_remove(fname);
	VIM_CLEAR(int_wordlist);
    }
}