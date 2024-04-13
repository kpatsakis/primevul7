static void *spl_ptr_llist_pop(spl_ptr_llist *llist TSRMLS_DC) /* {{{ */
{
	void                     *data;
	spl_ptr_llist_element    *tail = llist->tail;

	if (tail == NULL) {
		return NULL;
	}

	if (tail->prev) {
		tail->prev->next = NULL;
	} else {
		llist->head = NULL;
	}

	llist->tail = tail->prev;
	llist->count--;
	data = tail->data;

	if (llist->dtor) {
		llist->dtor(tail TSRMLS_CC);
	}

	tail->data = NULL;

	SPL_LLIST_DELREF(tail);

	return data;
}