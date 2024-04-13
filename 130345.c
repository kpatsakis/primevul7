jas_stream_t *jas_stream_fopen(const char *filename, const char *mode)
{
	jas_stream_t *stream;
	jas_stream_fileobj_t *obj;
	int openflags;

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

	/* Allocate space for the underlying file stream object. */
	if (!(obj = jas_malloc(sizeof(jas_stream_fileobj_t)))) {
		jas_stream_destroy(stream);
		return 0;
	}
	obj->fd = -1;
	obj->flags = 0;
	obj->pathname[0] = '\0';
	stream->obj_ = (void *) obj;

	/* Select the operations for a file stream object. */
	stream->ops_ = &jas_stream_fileops;

	/* Open the underlying file. */
	if ((obj->fd = open(filename, openflags, JAS_STREAM_PERMS)) < 0) {
		jas_stream_destroy(stream);
		return 0;
	}

	/* By default, use full buffering for this type of stream. */
	jas_stream_initbuf(stream, JAS_STREAM_FULLBUF, 0, 0);

	return stream;
}