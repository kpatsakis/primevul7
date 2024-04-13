static void spl_array_it_get_current_data(zend_object_iterator *iter, zval ***data TSRMLS_DC) /* {{{ */
{
	spl_array_it       *iterator = (spl_array_it *)iter;
	spl_array_object   *object   = iterator->object;
	HashTable          *aht      = spl_array_get_hash_table(object, 0 TSRMLS_CC);

	if (object->ar_flags & SPL_ARRAY_OVERLOADED_CURRENT) {
		zend_user_it_get_current_data(iter, data TSRMLS_CC);
	} else {
		if (zend_hash_get_current_data_ex(aht, (void**)data, &object->pos) == FAILURE) {
			*data = NULL;
		}
	}
}