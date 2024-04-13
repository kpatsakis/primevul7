jas_stream_t *jas_stream_tmpfile()
{
	jas_stream_t *stream;
	jas_stream_fileobj_t *obj;

	if (!(stream = jas_stream_create())) {
		return 0;
	}

	/* A temporary file stream is always opened for both reading and
	writing in binary mode. */
	stream->openmode_ = JAS_STREAM_READ | JAS_STREAM_WRITE | JAS_STREAM_BINARY;

	/* Allocate memory for the underlying temporary file object. */
	if (!(obj = jas_malloc(sizeof(jas_stream_fileobj_t)))) {
		jas_stream_destroy(stream);
		return 0;
	}
	obj->fd = -1;
	obj->flags = 0;
	obj->pathname[0] = '\0';
	stream->obj_ = obj;

	/* Choose a file name. */
	tmpnam(obj->pathname);

	/* Open the underlying file. */
	if ((obj->fd = open(obj->pathname, O_CREAT | O_EXCL | O_RDWR | O_TRUNC | O_BINARY,
	  JAS_STREAM_PERMS)) < 0) {
		jas_stream_destroy(stream);
		return 0;
	}

	/* Unlink the file so that it will disappear if the program
	terminates abnormally. */
	/* Under UNIX, one can unlink an open file and continue to do I/O
	on it.  Not all operating systems support this functionality, however.
	For example, under Microsoft Windows the unlink operation will fail,
	since the file is open. */
	if (unlink(obj->pathname)) {
		/* We will try unlinking the file again after it is closed. */
		obj->flags |= JAS_STREAM_FILEOBJ_DELONCLOSE;
	}

	/* Use full buffering. */
	jas_stream_initbuf(stream, JAS_STREAM_FULLBUF, 0, 0);

	stream->ops_ = &jas_stream_fileops;

	return stream;
}