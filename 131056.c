static void spl_ptr_llist_copy(spl_ptr_llist *from, spl_ptr_llist *to TSRMLS_DC) /* {{{ */
{
	spl_ptr_llist_element   *current = from->head, *next;
	spl_ptr_llist_ctor_func  ctor    = from->ctor;

	while (current) {
		next = current->next;

		if (ctor) {
			ctor(current TSRMLS_CC);
		}

		spl_ptr_llist_push(to, current->data TSRMLS_CC);
		current = next;
	}

}