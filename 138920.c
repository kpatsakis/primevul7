vpeekc_any(void)
{
    int		c;

    c = vpeekc();
    if (c == NUL && typebuf.tb_len > 0)
	c = ESC;
    return c;
}