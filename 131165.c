const char *get_git_dir(void)
{
	if (!git_dir)
		setup_git_env();
	return git_dir;
}