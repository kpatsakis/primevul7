static int spl_dllist_it_get_current_key(zend_object_iterator *iter, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC) /* {{{ */
{
	spl_dllist_it *iterator = (spl_dllist_it *)iter;

	*int_key = (ulong) iterator->traverse_position;
	return HASH_KEY_IS_LONG;
}