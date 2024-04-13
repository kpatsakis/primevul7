static void diff_summary(FILE *file, struct diff_filepair *p)
{
	switch(p->status) {
	case DIFF_STATUS_DELETED:
		show_file_mode_name(file, "delete", p->one);
		break;
	case DIFF_STATUS_ADDED:
		show_file_mode_name(file, "create", p->two);
		break;
	case DIFF_STATUS_COPIED:
		show_rename_copy(file, "copy", p);
		break;
	case DIFF_STATUS_RENAMED:
		show_rename_copy(file, "rename", p);
		break;
	default:
		if (p->score) {
			fputs(" rewrite ", file);
			write_name_quoted(p->two->path, file, ' ');
			fprintf(file, "(%d%%)\n", similarity_index(p));
		}
		show_mode_change(file, p, !p->score);
		break;
	}
}