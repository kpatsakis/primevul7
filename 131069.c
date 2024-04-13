static void spl_dllist_it_get_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC) /* {{{ */
{
	spl_dllist_it         *iterator = (spl_dllist_it *)iter;
	spl_ptr_llist_element *element  = iterator->traverse_pointer;

	if (element == NULL || element->data == NULL) {
		*data = NULL;
	} else {
		*data = (zval **)&element->data;
	}
}