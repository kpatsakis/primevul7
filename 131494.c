static void spl_array_it_dtor(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_array_it *iterator = (spl_array_it *)iter;

	zend_user_it_invalidate_current(iter TSRMLS_CC);
	zval_ptr_dtor((zval**)&iterator->intern.it.data);

	efree(iterator);
}