jas_stream_t *jpc_streamlist_remove(jpc_streamlist_t *streamlist, int streamno)
{
	jas_stream_t *stream;
	int i;
	if (streamno >= streamlist->numstreams) {
		abort();
	}
	stream = streamlist->streams[streamno];
	for (i = streamno + 1; i < streamlist->numstreams; ++i) {
		streamlist->streams[i - 1] = streamlist->streams[i];
	}
	--streamlist->numstreams;
	return stream;
}