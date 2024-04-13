static void free_diffstat_info(struct diffstat_t *diffstat)
{
	int i;
	for (i = 0; i < diffstat->nr; i++) {
		struct diffstat_file *f = diffstat->files[i];
		if (f->name != f->print_name)
			free(f->print_name);
		free(f->name);
		free(f->from_name);
		free(f);
	}
	free(diffstat->files);
}