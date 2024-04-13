tilde_expand_filename(const char *filename, uid_t uid)
{
	char *ret;

	if (tilde_expand(filename, uid, &ret) != 0)
		cleanup_exit(255);
	return ret;
}