updatescript(int c)
{
    static int	    count = 0;

    if (c && scriptout)
	putc(c, scriptout);
    if (c == 0 || (p_uc > 0 && ++count >= p_uc))
    {
	ml_sync_all(c == 0, TRUE);
	count = 0;
    }
}