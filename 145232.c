endoutput(
	FILE *fp
	)
{
	if (out_chars != 0)
		putc('\n', fp);
}