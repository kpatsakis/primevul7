int jas_stream_rewind(jas_stream_t *stream)
{
	return jas_stream_seek(stream, 0, SEEK_SET);
}