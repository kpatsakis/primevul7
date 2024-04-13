static void show_shortstats(struct diffstat_t* data, struct diff_options *options)
{
	int i, adds = 0, dels = 0, total_files = data->nr;

	if (data->nr == 0)
		return;

	for (i = 0; i < data->nr; i++) {
		if (!data->files[i]->is_binary &&
		    !data->files[i]->is_unmerged) {
			int added = data->files[i]->added;
			int deleted= data->files[i]->deleted;
			if (!data->files[i]->is_renamed &&
			    (added + deleted == 0)) {
				total_files--;
			} else {
				adds += added;
				dels += deleted;
			}
		}
	}
	fprintf(options->file, " %d files changed, %d insertions(+), %d deletions(-)\n",
	       total_files, adds, dels);
}