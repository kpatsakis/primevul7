static void sapi_remove_header(zend_llist *l, char *name, uint len) {
	sapi_header_struct *header;
	zend_llist_element *next;
	zend_llist_element *current=l->head;

	while (current) {
		header = (sapi_header_struct *)(current->data);
		next = current->next;
		if (header->header_len > len && header->header[len] == ':'
				&& !strncasecmp(header->header, name, len)) {
			if (current->prev) {
				current->prev->next = next;
			} else {
				l->head = next;
			}
			if (next) {
				next->prev = current->prev;
			} else {
				l->tail = current->prev;
			}
			sapi_free_header(header);
			efree(current);
			--l->count;
		}
		current = next;
	}
}