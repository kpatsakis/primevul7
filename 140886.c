count_var(
	const struct ctl_var *k
	)
{
	u_int c;

	if (NULL == k)
		return 0;

	c = 0;
	while (!(EOV & (k++)->flags))
		c++;

	ENSURE(c <= USHRT_MAX);
	return (u_short)c;
}