void diff_unmerge(struct diff_options *options,
		  const char *path,
		  unsigned mode, const unsigned char *sha1)
{
	struct diff_filespec *one, *two;

	if (options->prefix &&
	    strncmp(path, options->prefix, options->prefix_length))
		return;

	one = alloc_filespec(path);
	two = alloc_filespec(path);
	fill_filespec(one, sha1, mode);
	diff_queue(&diff_queued_diff, one, two)->is_unmerged = 1;
}