static int mem_close(jas_stream_obj_t *obj)
{
	jas_stream_memobj_t *m = (jas_stream_memobj_t *)obj;
	if (m->myalloc_ && m->buf_) {
		jas_free(m->buf_);
		m->buf_ = 0;
	}
	jas_free(obj);
	return 0;
}