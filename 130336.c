jas_stream_t *jas_stream_freopen(const char *path, const char *mode, FILE *fp)
{
	jas_stream_t *stream;
	int openflags;

	/* Eliminate compiler warning about unused variable. */
	path = 0;

	/* Allocate a stream object. */
	if (!(stream = jas_stream_create())) {
		return 0;
	}

	/* Parse the mode string. */
	stream->openmode_ = jas_strtoopenmode(mode);

	/* Determine the correct flags to use for opening the file. */
	if ((stream->openmode_ & JAS_STREAM_READ) &&
	  (stream->openmode_ & JAS_STREAM_WRITE)) {
		openflags = O_RDWR;
	} else if (stream->openmode_ & JAS_STREAM_READ) {
		openflags = O_RDONLY;
	} else if (stream->openmode_ & JAS_STREAM_WRITE) {
		openflags = O_WRONLY;
	} else {
		openflags = 0;
	}
	if (stream->openmode_ & JAS_STREAM_APPEND) {
		openflags |= O_APPEND;
	}
	if (stream->openmode_ & JAS_STREAM_BINARY) {
		openflags |= O_BINARY;
	}
	if (stream->openmode_ & JAS_STREAM_CREATE) {
		openflags |= O_CREAT | O_TRUNC;
	}

	stream->obj_ = JAS_CAST(void *, fp);

	/* Select the operations for a file stream object. */
	stream->ops_ = &jas_stream_sfileops;

	/* By default, use full buffering for this type of stream. */
	jas_stream_initbuf(stream, JAS_STREAM_FULLBUF, 0, 0);

	return stream;
}