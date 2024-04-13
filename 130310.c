long jas_stream_seek(jas_stream_t *stream, long offset, int origin)
{
	long newpos;

	/* The buffer cannot be in use for both reading and writing. */
	assert(!((stream->bufmode_ & JAS_STREAM_RDBUF) && (stream->bufmode_ &
	  JAS_STREAM_WRBUF)));

	/* Reset the EOF indicator (since we may not be at the EOF anymore). */
	stream->flags_ &= ~JAS_STREAM_EOF;

	if (stream->bufmode_ & JAS_STREAM_RDBUF) {
		if (origin == SEEK_CUR) {
			offset -= stream->cnt_;
		}
	} else if (stream->bufmode_ & JAS_STREAM_WRBUF) {
		if (jas_stream_flush(stream)) {
			return -1;
		}
	}
	stream->cnt_ = 0;
	stream->ptr_ = stream->bufstart_;
	stream->bufmode_ &= ~(JAS_STREAM_RDBUF | JAS_STREAM_WRBUF);

	if ((newpos = (*stream->ops_->seek_)(stream->obj_, offset, origin))
	  < 0) {
		return -1;
	}

	return newpos;
}