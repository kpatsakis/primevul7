jpc_dec_seg_t *jpc_seg_alloc()
{
	jpc_dec_seg_t *seg;

	if (!(seg = jas_malloc(sizeof(jpc_dec_seg_t)))) {
		return 0;
	}
	seg->prev = 0;
	seg->next = 0;
	seg->passno = -1;
	seg->numpasses = 0;
	seg->maxpasses = 0;
	seg->type = JPC_SEG_INVALID;
	seg->stream = 0;
	seg->cnt = 0;
	seg->complete = 0;
	seg->lyrno = -1;
	return seg;
}