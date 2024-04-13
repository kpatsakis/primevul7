int jas_stream_close(jas_stream_t *stream)
{
	/* Flush buffer if necessary. */
	jas_stream_flush(stream);

	/* Close the underlying stream object. */
	(*stream->ops_->close_)(stream->obj_);

	jas_stream_destroy(stream);

	return 0;
}