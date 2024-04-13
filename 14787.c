add_pack_plugin(char_u *fname, void *cookie)
{
    if (cookie != &APP_LOAD)
    {
	char_u	*buf = alloc(MAXPATHL);
	char_u	*p;
	int	found = FALSE;

	if (buf == NULL)
	    return;
	p = p_rtp;
	while (*p != NUL)
	{
	    copy_option_part(&p, buf, MAXPATHL, ",");
	    if (pathcmp((char *)buf, (char *)fname, -1) == 0)
	    {
		found = TRUE;
		break;
	    }
	}
	vim_free(buf);
	if (!found)
	    // directory is not yet in 'runtimepath', add it
	    if (add_pack_dir_to_rtp(fname) == FAIL)
		return;
    }

    if (cookie != &APP_ADD_DIR)
	load_pack_plugin(fname);
}