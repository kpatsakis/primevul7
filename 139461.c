static void jas_image_cmpt_destroy(jas_image_cmpt_t *cmpt)
{
	if (cmpt->stream_) {
		jas_stream_close(cmpt->stream_);
	}
	jas_free(cmpt);
}