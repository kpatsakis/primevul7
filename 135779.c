static void show_name(FILE *file,
		      const char *prefix, const char *name, int len,
		      const char *reset, const char *set)
{
	fprintf(file, " %s%s%-*s%s |", set, prefix, len, name, reset);
}