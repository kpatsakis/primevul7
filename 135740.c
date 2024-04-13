static void show_stats(struct diffstat_t* data, struct diff_options *options)
{
	int i, len, add, del, total, adds = 0, dels = 0;
	int max_change = 0, max_len = 0;
	int total_files = data->nr;
	int width, name_width;
	const char *reset, *set, *add_c, *del_c;

	if (data->nr == 0)
		return;

	width = options->stat_width ? options->stat_width : 80;
	name_width = options->stat_name_width ? options->stat_name_width : 50;

	/* Sanity: give at least 5 columns to the graph,
	 * but leave at least 10 columns for the name.
	 */
	if (width < name_width + 15) {
		if (name_width <= 25)
			width = name_width + 15;
		else
			name_width = width - 15;
	}

	/* Find the longest filename and max number of changes */
	reset = diff_get_color_opt(options, DIFF_RESET);
	set   = diff_get_color_opt(options, DIFF_PLAIN);
	add_c = diff_get_color_opt(options, DIFF_FILE_NEW);
	del_c = diff_get_color_opt(options, DIFF_FILE_OLD);

	for (i = 0; i < data->nr; i++) {
		struct diffstat_file *file = data->files[i];
		int change = file->added + file->deleted;
		fill_print_name(file);
		len = strlen(file->print_name);
		if (max_len < len)
			max_len = len;

		if (file->is_binary || file->is_unmerged)
			continue;
		if (max_change < change)
			max_change = change;
	}

	/* Compute the width of the graph part;
	 * 10 is for one blank at the beginning of the line plus
	 * " | count " between the name and the graph.
	 *
	 * From here on, name_width is the width of the name area,
	 * and width is the width of the graph area.
	 */
	name_width = (name_width < max_len) ? name_width : max_len;
	if (width < (name_width + 10) + max_change)
		width = width - (name_width + 10);
	else
		width = max_change;

	for (i = 0; i < data->nr; i++) {
		const char *prefix = "";
		char *name = data->files[i]->print_name;
		int added = data->files[i]->added;
		int deleted = data->files[i]->deleted;
		int name_len;

		/*
		 * "scale" the filename
		 */
		len = name_width;
		name_len = strlen(name);
		if (name_width < name_len) {
			char *slash;
			prefix = "...";
			len -= 3;
			name += name_len - len;
			slash = strchr(name, '/');
			if (slash)
				name = slash;
		}

		if (data->files[i]->is_binary) {
			show_name(options->file, prefix, name, len, reset, set);
			fprintf(options->file, "  Bin ");
			fprintf(options->file, "%s%d%s", del_c, deleted, reset);
			fprintf(options->file, " -> ");
			fprintf(options->file, "%s%d%s", add_c, added, reset);
			fprintf(options->file, " bytes");
			fprintf(options->file, "\n");
			continue;
		}
		else if (data->files[i]->is_unmerged) {
			show_name(options->file, prefix, name, len, reset, set);
			fprintf(options->file, "  Unmerged\n");
			continue;
		}
		else if (!data->files[i]->is_renamed &&
			 (added + deleted == 0)) {
			total_files--;
			continue;
		}

		/*
		 * scale the add/delete
		 */
		add = added;
		del = deleted;
		total = add + del;
		adds += add;
		dels += del;

		if (width <= max_change) {
			add = scale_linear(add, width, max_change);
			del = scale_linear(del, width, max_change);
			total = add + del;
		}
		show_name(options->file, prefix, name, len, reset, set);
		fprintf(options->file, "%5d ", added + deleted);
		show_graph(options->file, '+', add, add_c, reset);
		show_graph(options->file, '-', del, del_c, reset);
		fprintf(options->file, "\n");
	}
	fprintf(options->file,
	       "%s %d files changed, %d insertions(+), %d deletions(-)%s\n",
	       set, total_files, adds, dels, reset);
}