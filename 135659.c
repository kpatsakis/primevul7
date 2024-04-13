static unsigned char *deflate_it(char *data,
				 unsigned long size,
				 unsigned long *result_size)
{
	int bound;
	unsigned char *deflated;
	z_stream stream;

	memset(&stream, 0, sizeof(stream));
	deflateInit(&stream, zlib_compression_level);
	bound = deflateBound(&stream, size);
	deflated = xmalloc(bound);
	stream.next_out = deflated;
	stream.avail_out = bound;

	stream.next_in = (unsigned char *)data;
	stream.avail_in = size;
	while (deflate(&stream, Z_FINISH) == Z_OK)
		; /* nothing */
	deflateEnd(&stream);
	*result_size = stream.total_out;
	return deflated;
}