int jas_stream_ungetc(jas_stream_t *stream, int c)
{
	if (!stream->ptr_ || stream->ptr_ == stream->bufbase_) {
		return -1;
	}

	/* Reset the EOF indicator (since we now have at least one character
	  to read). */
	stream->flags_ &= ~JAS_STREAM_EOF;

	--stream->rwcnt_;
	--stream->ptr_;
	++stream->cnt_;
	*stream->ptr_ = c;
	return 0;
}