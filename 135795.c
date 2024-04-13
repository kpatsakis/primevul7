static int diff_filespec_is_identical(struct diff_filespec *one,
				      struct diff_filespec *two)
{
	if (S_ISGITLINK(one->mode))
		return 0;
	if (diff_populate_filespec(one, 0))
		return 0;
	if (diff_populate_filespec(two, 0))
		return 0;
	return !memcmp(one->data, two->data, one->size);
}