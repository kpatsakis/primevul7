vpeekc(void)
{
    if (old_char != -1)
	return old_char;
    return vgetorpeek(FALSE);
}