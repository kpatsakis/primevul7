privsep_enabled(void)
{
	if (pfd < 0)
		return 0;
	return 1;
}