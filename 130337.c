static long mem_seek(jas_stream_obj_t *obj, long offset, int origin)
{
	jas_stream_memobj_t *m = (jas_stream_memobj_t *)obj;
	long newpos;

	switch (origin) {
	case SEEK_SET:
		newpos = offset;
		break;
	case SEEK_END:
		newpos = m->len_ - offset;
		break;
	case SEEK_CUR:
		newpos = m->pos_ + offset;
		break;
	default:
		abort();
		break;
	}
	if (newpos < 0) {
		return -1;
	}
	m->pos_ = newpos;

	return m->pos_;
}