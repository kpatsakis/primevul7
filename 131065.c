static void spl_dllist_object_free_storage(void *object TSRMLS_DC) /* {{{ */
{
	spl_dllist_object *intern = (spl_dllist_object *)object;
	zval              *tmp    = NULL;

	zend_object_std_dtor(&intern->std TSRMLS_CC);

	while(intern->llist->count > 0) {
		tmp = (zval *)spl_ptr_llist_pop(intern->llist TSRMLS_CC);
		zval_ptr_dtor(&tmp);
	}

	spl_ptr_llist_destroy(intern->llist TSRMLS_CC);
	SPL_LLIST_CHECK_DELREF(intern->traverse_pointer);
	zval_ptr_dtor(&intern->retval);

	if (intern->debug_info != NULL) {
		zend_hash_destroy(intern->debug_info);
		efree(intern->debug_info);
	}

	efree(object);
}