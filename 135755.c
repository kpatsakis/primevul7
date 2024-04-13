static void show_file_mode_name(FILE *file, const char *newdelete, struct diff_filespec *fs)
{
	if (fs->mode)
		fprintf(file, " %s mode %06o ", newdelete, fs->mode);
	else
		fprintf(file, " %s ", newdelete);
	write_name_quoted(fs->path, file, '\n');
}