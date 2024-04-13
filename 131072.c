static void spl_ptr_llist_unshift(spl_ptr_llist *llist, void *data TSRMLS_DC) /* {{{ */
{
	spl_ptr_llist_element *elem = emalloc(sizeof(spl_ptr_llist_element));

	elem->data = data;
	elem->rc   = 1;
	elem->prev = NULL;
	elem->next = llist->head;

	if (llist->head) {
		llist->head->prev = elem;
	} else {
		llist->tail = elem;
	}

	llist->head = elem;
	llist->count++;

	if (llist->ctor) {
		llist->ctor(elem TSRMLS_CC);
	}
}