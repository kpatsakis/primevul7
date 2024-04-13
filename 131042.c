static void spl_ptr_llist_push(spl_ptr_llist *llist, void *data TSRMLS_DC) /* {{{ */
{
	spl_ptr_llist_element *elem = emalloc(sizeof(spl_ptr_llist_element));

	elem->data = data;
	elem->rc   = 1;
	elem->prev = llist->tail;
	elem->next = NULL;

	if (llist->tail) {
		llist->tail->next = elem;
	} else {
		llist->head = elem;
	}

	llist->tail = elem;
	llist->count++;

	if (llist->ctor) {
		llist->ctor(elem TSRMLS_CC);
	}
}