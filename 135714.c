static void show_numstat(struct diffstat_t* data, struct diff_options *options)
{
	int i;

	if (data->nr == 0)
		return;

	for (i = 0; i < data->nr; i++) {
		struct diffstat_file *file = data->files[i];

		if (file->is_binary)
			fprintf(options->file, "-\t-\t");
		else
			fprintf(options->file,
				"%d\t%d\t", file->added, file->deleted);
		if (options->line_termination) {
			fill_print_name(file);
			if (!file->is_renamed)
				write_name_quoted(file->name, options->file,
						  options->line_termination);
			else {
				fputs(file->print_name, options->file);
				putc(options->line_termination, options->file);
			}
		} else {
			if (file->is_renamed) {
				putc('\0', options->file);
				write_name_quoted(file->from_name, options->file, '\0');
			}
			write_name_quoted(file->name, options->file, '\0');
		}
	}
}