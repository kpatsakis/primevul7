long jas_stream_length(jas_stream_t *stream)
{
	long oldpos;
	long pos;
	if ((oldpos = jas_stream_tell(stream)) < 0) {
		return -1;
	}
	if (jas_stream_seek(stream, 0, SEEK_END) < 0) {
		return -1;
	}
	if ((pos = jas_stream_tell(stream)) < 0) {
		return -1;
	}
	if (jas_stream_seek(stream, oldpos, SEEK_SET) < 0) {
		return -1;
	}
	return pos;
}