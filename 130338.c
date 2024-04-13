jas_stream_t *jas_stream_fdopen(int fd, const char *mode)
{
	jas_stream_t *stream;
	jas_stream_fileobj_t *obj;

	/* Allocate a stream object. */
	if (!(stream = jas_stream_create())) {
		return 0;
	}

	/* Parse the mode string. */
	stream->openmode_ = jas_strtoopenmode(mode);

#if defined(WIN32)
	/* Argh!!!  Someone ought to banish text mode (i.e., O_TEXT) to the
	  greatest depths of purgatory! */
	/* Ensure that the file descriptor is in binary mode, if the caller
	  has specified the binary mode flag.  Arguably, the caller ought to
	  take care of this, but text mode is a ugly wart anyways, so we save
	  the caller some grief by handling this within the stream library. */
	/* This ugliness is mainly for the benefit of those who run the
	  JasPer software under Windows from shells that insist on opening
	  files in text mode.  For example, in the Cygwin environment,
	  shells often open files in text mode when I/O redirection is
	  used.  Grr... */
	if (stream->openmode_ & JAS_STREAM_BINARY) {
		setmode(fd, O_BINARY);
	}
#endif

	/* Allocate space for the underlying file stream object. */
	if (!(obj = jas_malloc(sizeof(jas_stream_fileobj_t)))) {
		jas_stream_destroy(stream);
		return 0;
	}
	obj->fd = fd;
	obj->flags = 0;
	obj->pathname[0] = '\0';
	stream->obj_ = (void *) obj;

	/* Do not close the underlying file descriptor when the stream is
	closed. */
	obj->flags |= JAS_STREAM_FILEOBJ_NOCLOSE;

	/* By default, use full buffering for this type of stream. */
	jas_stream_initbuf(stream, JAS_STREAM_FULLBUF, 0, 0);

	/* Select the operations for a file stream object. */
	stream->ops_ = &jas_stream_fileops;

	return stream;
}