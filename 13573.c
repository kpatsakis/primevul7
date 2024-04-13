skip_to_start(int c, colnr_T *colp)
{
    char_u *s;

    // Used often, do some work to avoid call overhead.
    if (!rex.reg_ic && !has_mbyte)
	s = vim_strbyte(rex.line + *colp, c);
    else
	s = cstrchr(rex.line + *colp, c);
    if (s == NULL)
	return FAIL;
    *colp = (int)(s - rex.line);
    return OK;
}