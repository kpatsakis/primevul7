do_in_path_and_pp(
    char_u	*path,
    char_u	*name,
    int		flags,
    void	(*callback)(char_u *fname, void *ck),
    void	*cookie)
{
    int		done = FAIL;
    char_u	*s;
    int		len;
    char	*start_dir = "pack/*/start/*/%s";
    char	*opt_dir = "pack/*/opt/*/%s";

    if ((flags & DIP_NORTP) == 0)
	done = do_in_path(path, name, flags, callback, cookie);

    if ((done == FAIL || (flags & DIP_ALL)) && (flags & DIP_START))
    {
	len = (int)(STRLEN(start_dir) + STRLEN(name));
	s = alloc(len);
	if (s == NULL)
	    return FAIL;
	vim_snprintf((char *)s, len, start_dir, name);
	done = do_in_path(p_pp, s, flags, callback, cookie);
	vim_free(s);
    }

    if ((done == FAIL || (flags & DIP_ALL)) && (flags & DIP_OPT))
    {
	len = (int)(STRLEN(opt_dir) + STRLEN(name));
	s = alloc(len);
	if (s == NULL)
	    return FAIL;
	vim_snprintf((char *)s, len, opt_dir, name);
	done = do_in_path(p_pp, s, flags, callback, cookie);
	vim_free(s);
    }

    return done;
}