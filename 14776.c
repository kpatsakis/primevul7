do_source(
    char_u	*fname,
    int		check_other,	    // check for .vimrc and _vimrc
    int		is_vimrc,	    // DOSO_ value
    int		*ret_sid UNUSED)
{
    return do_source_ext(fname, check_other, is_vimrc, ret_sid, NULL, FALSE);
}