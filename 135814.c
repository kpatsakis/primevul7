static void show_graph(FILE *file, char ch, int cnt, const char *set, const char *reset)
{
	if (cnt <= 0)
		return;
	fprintf(file, "%s", set);
	while (cnt--)
		putc(ch, file);
	fprintf(file, "%s", reset);
}