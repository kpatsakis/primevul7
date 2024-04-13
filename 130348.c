int jas_stream_flush(jas_stream_t *stream)
{
	if (stream->bufmode_ & JAS_STREAM_RDBUF) {
		return 0;
	}
	return jas_stream_flushbuf(stream, EOF);
}