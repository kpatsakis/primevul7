cooked(
	struct parse *pcmd,
	FILE *fp
	)
{
	rawmode = 0;
	(void) fprintf(fp, "Output set to cooked\n");
	return;
}