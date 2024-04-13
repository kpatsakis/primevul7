static void *spl_ptr_llist_shift(spl_ptr_llist *llist TSRMLS_DC) /* {{{ */
{
	void                    *data;
	spl_ptr_llist_element   *head = llist->head;

	if (head == NULL) {
		return NULL;
	}

	if (head->next) {
		head->next->prev = NULL;
	} else {
		llist->tail = NULL;
	}

	llist->head = head->next;
	llist->count--;
	data = head->data;

	if (llist->dtor) {
		llist->dtor(head TSRMLS_CC);
	}
	head->data = NULL;

	SPL_LLIST_DELREF(head);

	return data;
}