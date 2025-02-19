int jpc_streamlist_insert(jpc_streamlist_t *streamlist, int streamno,
  jas_stream_t *stream)
{
	jas_stream_t **newstreams;
	int newmaxstreams;
	int i;
	/* Grow the array of streams if necessary. */
	if (streamlist->numstreams >= streamlist->maxstreams) {
		newmaxstreams = streamlist->maxstreams + 1024;
		if (!(newstreams = jas_realloc2(streamlist->streams,
		  (newmaxstreams + 1024), sizeof(jas_stream_t *)))) {
			return -1;
		}
		for (i = streamlist->numstreams; i < streamlist->maxstreams; ++i) {
			streamlist->streams[i] = 0;
		}
		streamlist->maxstreams = newmaxstreams;
		streamlist->streams = newstreams;
	}
	if (streamno != streamlist->numstreams) {
		/* Can only handle insertion at start of list. */
		return -1;
	}
	streamlist->streams[streamno] = stream;
	++streamlist->numstreams;
	return 0;
}