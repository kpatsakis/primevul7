spell_reload(void)
{
    win_T	*wp;

    // Initialize the table for spell_iswordp().
    init_spell_chartab();

    // Unload all allocated memory.
    spell_free_all();

    // Go through all buffers and handle 'spelllang'.
    FOR_ALL_WINDOWS(wp)
    {
	// Only load the wordlists when 'spelllang' is set and there is a
	// window for this buffer in which 'spell' is set.
	if (*wp->w_s->b_p_spl != NUL)
	{
		if (wp->w_p_spell)
		{
		    (void)did_set_spelllang(wp);
		    break;
		}
	}
    }
}