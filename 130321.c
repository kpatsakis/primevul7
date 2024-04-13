static jas_stream_t *jas_stream_create()
{
	jas_stream_t *stream;

	if (!(stream = jas_malloc(sizeof(jas_stream_t)))) {
		return 0;
	}
	stream->openmode_ = 0;
	stream->bufmode_ = 0;
	stream->flags_ = 0;
	stream->bufbase_ = 0;
	stream->bufstart_ = 0;
	stream->bufsize_ = 0;
	stream->ptr_ = 0;
	stream->cnt_ = 0;
	stream->ops_ = 0;
	stream->obj_ = 0;
	stream->rwcnt_ = 0;
	stream->rwlimit_ = -1;

	return stream;
}