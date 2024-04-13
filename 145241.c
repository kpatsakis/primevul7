raw(
	struct parse *pcmd,
	FILE *fp
	)
{
	rawmode = 1;
	(void) fprintf(fp, "Output set to raw\n");
}