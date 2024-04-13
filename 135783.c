static void show_rename_copy(FILE *file, const char *renamecopy, struct diff_filepair *p)
{
	char *names = pprint_rename(p->one->path, p->two->path);

	fprintf(file, " %s %s (%d%%)\n", renamecopy, names, similarity_index(p));
	free(names);
	show_mode_change(file, p, 0);
}