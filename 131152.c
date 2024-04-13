char *get_index_file(void)
{
	if (!git_index_file)
		setup_git_env();
	return git_index_file;
}