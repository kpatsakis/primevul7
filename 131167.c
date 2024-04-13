open_spellbuf(void)
{
    buf_T	*buf;

    buf = ALLOC_CLEAR_ONE(buf_T);
    if (buf != NULL)
    {
	buf->b_spell = TRUE;
	buf->b_p_swf = TRUE;	// may create a swap file
#ifdef FEAT_CRYPT
	buf->b_p_key = empty_option;
#endif
	ml_open(buf);
	ml_open_file(buf);	// create swap file now
    }
    return buf;
}