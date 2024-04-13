static int populate_from_stdin(struct diff_filespec *s)
{
	struct strbuf buf;
	size_t size = 0;

	strbuf_init(&buf, 0);
	if (strbuf_read(&buf, 0, 0) < 0)
		return error("error while reading from stdin %s",
				     strerror(errno));

	s->should_munmap = 0;
	s->data = strbuf_detach(&buf, &size);
	s->size = size;
	s->should_free = 1;
	return 0;
}