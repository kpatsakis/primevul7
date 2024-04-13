static int spl_array_it_valid(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_array_it       *iterator = (spl_array_it *)iter;
	spl_array_object   *object   = iterator->object;
	HashTable          *aht      = spl_array_get_hash_table(object, 0 TSRMLS_CC);

	if (object->ar_flags & SPL_ARRAY_OVERLOADED_VALID) {
		return zend_user_it_valid(iter TSRMLS_CC);
	} else {
		if (spl_array_object_verify_pos_ex(object, aht, "ArrayIterator::valid(): " TSRMLS_CC) == FAILURE) {
			return FAILURE;
		}

		return zend_hash_has_more_elements_ex(aht, &object->pos);
	}
}