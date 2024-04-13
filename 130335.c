int jas_stream_flushbuf(jas_stream_t *stream, int c)
{
	int len;
	int n;

	/* The stream should not be in an error or EOF state. */
	if ((stream->flags_ & (JAS_STREAM_ERRMASK)) != 0) {
		return EOF;
	}

	/* The stream must be open for writing. */
	if ((stream->openmode_ & (JAS_STREAM_WRITE | JAS_STREAM_APPEND)) == 0) {
		return EOF;
	}

	/* The buffer should not currently be in use for reading. */
	assert(!(stream->bufmode_ & JAS_STREAM_RDBUF));

	/* Note: Do not use the quantity stream->cnt to determine the number
	of characters in the buffer!  Depending on how this function was
	called, the stream->cnt value may be "off-by-one". */
	len = stream->ptr_ - stream->bufstart_;
	if (len > 0) {
		n = (*stream->ops_->write_)(stream->obj_, (char *)
		  stream->bufstart_, len);
		if (n != len) {
			stream->flags_ |= JAS_STREAM_ERR;
			return EOF;
		}
	}
	stream->cnt_ = stream->bufsize_;
	stream->ptr_ = stream->bufstart_;

	stream->bufmode_ |= JAS_STREAM_WRBUF;

	if (c != EOF) {
		assert(stream->cnt_ > 0);
		return jas_stream_putc2(stream, c);
	}

	return 0;
}