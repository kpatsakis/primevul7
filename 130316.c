int jas_stream_putc_func(jas_stream_t *stream, int c)
{
	assert(stream->ptr_ - stream->bufstart_ <= stream->bufsize_);
	return jas_stream_putc_macro(stream, c);
}