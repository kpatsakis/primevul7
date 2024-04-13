long jas_stream_setrwcount(jas_stream_t *stream, long rwcnt)
{
	int old;

	old = stream->rwcnt_;
	stream->rwcnt_ = rwcnt;
	return old;
}