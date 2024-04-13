add_map(char_u *map, int mode)
{
    char_u	*s;
    char_u	*cpo_save = p_cpo;

    p_cpo = (char_u *)"";	/* Allow <> notation */
    s = vim_strsave(map);
    if (s != NULL)
    {
	(void)do_map(0, s, mode, FALSE);
	vim_free(s);
    }
    p_cpo = cpo_save;
}