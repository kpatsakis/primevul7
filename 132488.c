char **lxc_string_split(const char *string, char _sep)
{
	char *token, *str;
	char sep[2] = {_sep, '\0'};
	char **tmp = NULL, **result = NULL;
	size_t result_capacity = 0;
	size_t result_count = 0;
	int r, saved_errno;
	size_t len;

	if (!string)
		return calloc(1, sizeof(char *));

	len = strlen(string);
	str = alloca(len + 1);
	(void)strlcpy(str, string, len + 1);

	lxc_iterate_parts(token, str, sep) {
		r = lxc_grow_array((void ***)&result, &result_capacity, result_count + 1, 16);
		if (r < 0)
			goto error_out;

		result[result_count] = strdup(token);
		if (!result[result_count])
			goto error_out;

		result_count++;
	}

	/* if we allocated too much, reduce it */
	tmp = realloc(result, (result_count + 1) * sizeof(char *));
	if (!tmp)
		goto error_out;

	result = tmp;

	/* Make sure we don't return uninitialized memory. */
	if (result_count == 0)
		*result = NULL;

	return result;

error_out:
	saved_errno = errno;
	lxc_free_array((void **)result, free);
	errno = saved_errno;
	return NULL;
}