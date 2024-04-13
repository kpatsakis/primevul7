const char *prefix_path(const char *prefix, int len, const char *path)
{
	const char *orig = path;
	char *sanitized = xmalloc(len + strlen(path) + 1);
	if (is_absolute_path(orig))
		strcpy(sanitized, path);
	else {
		if (len)
			memcpy(sanitized, prefix, len);
		strcpy(sanitized + len, path);
	}
	if (sanitary_path_copy(sanitized, sanitized))
		goto error_out;
	if (is_absolute_path(orig)) {
		const char *work_tree = get_git_work_tree();
		size_t len = strlen(work_tree);
		size_t total = strlen(sanitized) + 1;
		if (strncmp(sanitized, work_tree, len) ||
		    (sanitized[len] != '\0' && sanitized[len] != '/')) {
		error_out:
			error("'%s' is outside repository", orig);
			free(sanitized);
			return NULL;
		}
		if (sanitized[len] == '/')
			len++;
		memmove(sanitized, sanitized + len, total - len);
	}
	return sanitized;
}