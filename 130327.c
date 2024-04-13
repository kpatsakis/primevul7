int jas_stream_fillbuf(jas_stream_t *stream, int getflag)
{
	int c;

	/* The stream must not be in an error or EOF state. */
	if ((stream->flags_ & (JAS_STREAM_ERRMASK)) != 0) {
		return EOF;
	}

	/* The stream must be open for reading. */
	if ((stream->openmode_ & JAS_STREAM_READ) == 0) {
		return EOF;
	}

	/* Make a half-hearted attempt to confirm that the buffer is not
	currently being used for writing.  This check is not intended
	to be foolproof! */
	assert((stream->bufmode_ & JAS_STREAM_WRBUF) == 0);

	assert(stream->ptr_ - stream->bufstart_ <= stream->bufsize_);

	/* Mark the buffer as being used for reading. */
	stream->bufmode_ |= JAS_STREAM_RDBUF;

	/* Read new data into the buffer. */
	stream->ptr_ = stream->bufstart_;
	if ((stream->cnt_ = (*stream->ops_->read_)(stream->obj_,
	  (char *) stream->bufstart_, stream->bufsize_)) <= 0) {
		if (stream->cnt_ < 0) {
			stream->flags_ |= JAS_STREAM_ERR;
		} else {
			stream->flags_ |= JAS_STREAM_EOF;
		}
		stream->cnt_ = 0;
		return EOF;
	}

	assert(stream->cnt_ > 0);
	/* Get or peek at the first character in the buffer. */
	c = (getflag) ? jas_stream_getc2(stream) : (*stream->ptr_);

	return c;
}