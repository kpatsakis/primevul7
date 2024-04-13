close_spellbuf(buf_T *buf)
{
    if (buf != NULL)
    {
	ml_close(buf, TRUE);
	vim_free(buf);
    }
}