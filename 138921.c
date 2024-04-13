map_to_exists(char_u *str, char_u *modechars, int abbr)
{
    int		mode = 0;
    char_u	*rhs;
    char_u	*buf;
    int		retval;

    rhs = replace_termcodes(str, &buf, FALSE, TRUE, FALSE);

    if (vim_strchr(modechars, 'n') != NULL)
	mode |= NORMAL;
    if (vim_strchr(modechars, 'v') != NULL)
	mode |= VISUAL + SELECTMODE;
    if (vim_strchr(modechars, 'x') != NULL)
	mode |= VISUAL;
    if (vim_strchr(modechars, 's') != NULL)
	mode |= SELECTMODE;
    if (vim_strchr(modechars, 'o') != NULL)
	mode |= OP_PENDING;
    if (vim_strchr(modechars, 'i') != NULL)
	mode |= INSERT;
    if (vim_strchr(modechars, 'l') != NULL)
	mode |= LANGMAP;
    if (vim_strchr(modechars, 'c') != NULL)
	mode |= CMDLINE;

    retval = map_to_exists_mode(rhs, mode, abbr);
    vim_free(buf);

    return retval;
}