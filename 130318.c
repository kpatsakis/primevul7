static int mem_write(jas_stream_obj_t *obj, char *buf, int cnt)
{
	int n;
	int ret;
	jas_stream_memobj_t *m = (jas_stream_memobj_t *)obj;
	long newbufsize;
	long newpos;

	newpos = m->pos_ + cnt;
	if (newpos > m->bufsize_ && m->growable_) {
		newbufsize = m->bufsize_;
		while (newbufsize < newpos) {
			newbufsize <<= 1;
			assert(newbufsize >= 0);
		}
		if (mem_resize(m, newbufsize)) {
			return -1;
		}
	}
	if (m->pos_ > m->len_) {
		/* The current position is beyond the end of the file, so
		  pad the file to the current position with zeros. */
		n = JAS_MIN(m->pos_, m->bufsize_) - m->len_;
		if (n > 0) {
			memset(&m->buf_[m->len_], 0, n);
			m->len_ += n;
		}
		if (m->pos_ != m->len_) {
			/* The buffer is not big enough. */
			return 0;
		}
	}
	n = m->bufsize_ - m->pos_;
	ret = JAS_MIN(n, cnt);
	if (ret > 0) {
		memcpy(&m->buf_[m->pos_], buf, ret);
		m->pos_ += ret;
	}
	if (m->pos_ > m->len_) {
		m->len_ = m->pos_;
	}
assert(ret == cnt);
	return ret;
}