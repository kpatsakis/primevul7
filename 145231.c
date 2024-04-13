asciize(
	int length,
	char *data,
	FILE *fp
	)
{
	makeascii(length, data, fp);
	putc('\n', fp);
}