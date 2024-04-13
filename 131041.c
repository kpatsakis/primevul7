zend_object_iterator *spl_dllist_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC) /* {{{ */
{
	spl_dllist_it     *iterator;
	spl_dllist_object *dllist_object = (spl_dllist_object*)zend_object_store_get_object(object TSRMLS_CC);

	if (by_ref) {
		zend_throw_exception(spl_ce_RuntimeException, "An iterator cannot be used with foreach by reference", 0 TSRMLS_CC);
		return NULL;
	}

	Z_ADDREF_P(object);

	iterator                     = emalloc(sizeof(spl_dllist_it));
	iterator->intern.it.data     = (void*)object;
	iterator->intern.it.funcs    = &spl_dllist_it_funcs;
	iterator->intern.ce          = ce;
	iterator->intern.value       = NULL;
	iterator->traverse_position  = dllist_object->traverse_position;
	iterator->traverse_pointer   = dllist_object->traverse_pointer;
	iterator->flags              = dllist_object->flags & SPL_DLLIST_IT_MASK;
	iterator->object             = dllist_object;

	SPL_LLIST_CHECK_ADDREF(iterator->traverse_pointer);


	return (zend_object_iterator*)iterator;
}