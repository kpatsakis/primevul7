
private int
handle_annotation(struct magic_set *ms, struct magic *m, int firstline)
{
	if ((ms->flags & MAGIC_APPLE) && m->apple[0]) {
		if (print_sep(ms, firstline) == -1)
			return -1;
		if (file_printf(ms, "%.8s", m->apple) == -1)
			return -1;
		return 1;
	}
	if ((ms->flags & MAGIC_EXTENSION) && m->ext[0]) {
		if (print_sep(ms, firstline) == -1)
			return -1;
		if (file_printf(ms, "%s", m->ext) == -1)
			return -1;
		return 1;
	}
	if ((ms->flags & MAGIC_MIME_TYPE) && m->mimetype[0]) {
		char buf[1024];
		const char *p;
		if (print_sep(ms, firstline) == -1)
			return -1;
		if (varexpand(ms, buf, sizeof(buf), m->mimetype) == -1)
			p = m->mimetype;
		else
			p = buf;
		if (file_printf(ms, "%s", p) == -1)
			return -1;
		return 1;
	}