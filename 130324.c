int jas_stream_isseekable(jas_stream_t *stream)
{
	if (stream->ops_ == &jas_stream_memops) {
		return 1;
	} else if (stream->ops_ == &jas_stream_fileops) {
		if ((*stream->ops_->seek_)(stream->obj_, 0, SEEK_CUR) < 0) {
			return 0;
		}
		return 1;
	} else {
		return 0;
	}
}