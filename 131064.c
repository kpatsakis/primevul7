static void spl_ptr_llist_zval_dtor(spl_ptr_llist_element *elem TSRMLS_DC) { /* {{{ */
	if (elem->data) {
		zval_ptr_dtor((zval **)&elem->data);
	}
}