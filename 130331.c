int jas_stream_getc_func(jas_stream_t *stream)
{
	assert(stream->ptr_ - stream->bufbase_ <= stream->bufsize_ +
	  JAS_STREAM_MAXPUTBACK);
	return jas_stream_getc_macro(stream);
}