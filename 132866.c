void jpc_seglist_remove(jpc_dec_seglist_t *list, jpc_dec_seg_t *seg)
{
	jpc_dec_seg_t *prev;
	jpc_dec_seg_t *next;

	prev = seg->prev;
	next = seg->next;
	if (prev) {
		prev->next = next;
	} else {
		list->head = next;
	}
	if (next) {
		next->prev = prev;
	} else {
		list->tail = prev;
	}
	seg->prev = 0;
	seg->next = 0;
}