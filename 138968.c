safe_vgetc(void)
{
    int	c;

    c = vgetc();
    if (c == NUL)
	c = get_keystroke();
    return c;
}