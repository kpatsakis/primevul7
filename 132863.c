void jpc_seg_destroy(jpc_dec_seg_t *seg)
{
	if (seg->stream) {
		jas_stream_close(seg->stream);
	}
	jas_free(seg);
}