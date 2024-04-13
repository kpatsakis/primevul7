static void diff_filespec_check_attr(struct diff_filespec *one)
{
	struct git_attr_check attr_diff_check;
	int check_from_data = 0;

	if (one->checked_attr)
		return;

	setup_diff_attr_check(&attr_diff_check);
	one->is_binary = 0;
	one->funcname_pattern_ident = NULL;

	if (!git_checkattr(one->path, 1, &attr_diff_check)) {
		const char *value;

		/* binaryness */
		value = attr_diff_check.value;
		if (ATTR_TRUE(value))
			;
		else if (ATTR_FALSE(value))
			one->is_binary = 1;
		else
			check_from_data = 1;

		/* funcname pattern ident */
		if (ATTR_TRUE(value) || ATTR_FALSE(value) || ATTR_UNSET(value))
			;
		else
			one->funcname_pattern_ident = value;
	}

	if (check_from_data) {
		if (!one->data && DIFF_FILE_VALID(one))
			diff_populate_filespec(one, 0);

		if (one->data)
			one->is_binary = buffer_is_binary(one->data, one->size);
	}
}