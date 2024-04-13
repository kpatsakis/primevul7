int diff_filespec_is_binary(struct diff_filespec *one)
{
	diff_filespec_check_attr(one);
	return one->is_binary;
}