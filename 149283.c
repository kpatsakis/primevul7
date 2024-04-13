
private int
msetoffset(struct magic_set *ms, struct magic *m, struct buffer *bb,
    const struct buffer *b, size_t o, unsigned int cont_level)
{
	if (m->offset < 0) {
		if (cont_level > 0) {
			if (m->flag & (OFFADD|INDIROFFADD))
				goto normal;
#if 0
			file_error(ms, 0, "negative offset %d at continuation"
			    "level %u", m->offset, cont_level);
			return -1;
#endif
		}
		if (buffer_fill(b) == -1)
			return -1;
		if (o != 0) {
			// Not yet!
			file_magerror(ms, "non zero offset %" SIZE_T_FORMAT
			    "u at level %u", o, cont_level);
			return -1;
		}
		if ((size_t)-m->offset > b->elen)
			return -1;
		buffer_init(bb, -1, b->ebuf, b->elen);
		ms->eoffset = ms->offset = (int32_t)(b->elen + m->offset);
	} else {
		if (cont_level == 0) {
normal:
			// XXX: Pass real fd, then who frees bb?
			buffer_init(bb, -1, b->fbuf, b->flen);
			ms->offset = m->offset;
			ms->eoffset = 0;
		} else {
			ms->offset = ms->eoffset + m->offset;
		}
	}
	if ((ms->flags & MAGIC_DEBUG) != 0) {
		fprintf(stderr, "bb=[%p,%" SIZE_T_FORMAT "u], %d [b=%p,%"
		    SIZE_T_FORMAT "u], [o=%#x, c=%d]\n",
		    bb->fbuf, bb->flen, ms->offset, b->fbuf, b->flen,
		    m->offset, cont_level);
	}