static void spl_ptr_llist_destroy(spl_ptr_llist *llist TSRMLS_DC) /* {{{ */
{
	spl_ptr_llist_element   *current = llist->head, *next;
	spl_ptr_llist_dtor_func  dtor    = llist->dtor;

	while (current) {
		next = current->next;
		if(current && dtor) {
			dtor(current TSRMLS_CC);
		}
		SPL_LLIST_DELREF(current);
		current = next;
	}

	efree(llist);
}