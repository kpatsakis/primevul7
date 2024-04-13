spell_to_word_end(char_u *start, win_T *win)
{
    char_u  *p = start;

    while (*p != NUL && spell_iswordp(p, win))
	MB_PTR_ADV(p);
    return p;
}