static void spl_ptr_llist_zval_ctor(spl_ptr_llist_element *elem TSRMLS_DC) { /* {{{ */
	Z_ADDREF_P((zval *)elem->data);
}