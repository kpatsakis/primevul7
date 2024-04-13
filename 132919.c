void jpc_seglist_insert(jpc_dec_seglist_t *list, jpc_dec_seg_t *ins, jpc_dec_seg_t *node)
{
	jpc_dec_seg_t *prev;
	jpc_dec_seg_t *next;

	prev = ins;
	node->prev = prev;
	next = prev ? (prev->next) : 0;
	node->prev = prev;
	node->next = next;
	if (prev) {
		prev->next = node;
	} else {
		list->head = node;
	}
	if (next) {
		next->prev = node;
	} else {
		list->tail = node;
	}
}