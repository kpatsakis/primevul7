make_room_in_stream (MonoDynamicStream *stream, int size)
{
	if (size <= stream->alloc_size)
		return;
	
	while (stream->alloc_size <= size) {
		if (stream->alloc_size < 4096)
			stream->alloc_size = 4096;
		else
			stream->alloc_size *= 2;
	}
	
	stream->data = g_realloc (stream->data, stream->alloc_size);
}