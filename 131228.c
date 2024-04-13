spell_word_start(int startcol)
{
    char_u	*line;
    char_u	*p;
    int		col = 0;

    if (no_spell_checking(curwin))
	return startcol;

    // Find a word character before "startcol".
    line = ml_get_curline();
    for (p = line + startcol; p > line; )
    {
	MB_PTR_BACK(line, p);
	if (spell_iswordp_nmw(p, curwin))
	    break;
    }

    // Go back to start of the word.
    while (p > line)
    {
	col = (int)(p - line);
	MB_PTR_BACK(line, p);
	if (!spell_iswordp(p, curwin))
	    break;
	col = 0;
    }

    return col;
}