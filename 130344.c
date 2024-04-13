long jas_stream_tell(jas_stream_t *stream)
{
	int adjust;
	int offset;

	if (stream->bufmode_ & JAS_STREAM_RDBUF) {
		adjust = -stream->cnt_;
	} else if (stream->bufmode_ & JAS_STREAM_WRBUF) {
		adjust = stream->ptr_ - stream->bufstart_;
	} else {
		adjust = 0;
	}

	if ((offset = (*stream->ops_->seek_)(stream->obj_, 0, SEEK_CUR)) < 0) {
		return -1;
	}

	return offset + adjust;
}