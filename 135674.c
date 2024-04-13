static int diff_populate_gitlink(struct diff_filespec *s, int size_only)
{
	int len;
	char *data = xmalloc(100);
	len = snprintf(data, 100,
		"Subproject commit %s\n", sha1_to_hex(s->sha1));
	s->data = data;
	s->size = len;
	s->should_free = 1;
	if (size_only) {
		s->data = NULL;
		free(data);
	}
	return 0;
}