/* {{{ date_period_it_current_key */
static int date_period_it_current_key(zend_object_iterator *iter, char **str_key, uint *str_key_len, ulong *int_key TSRMLS_DC)
{
	date_period_it   *iterator = (date_period_it *)iter;
	*int_key = iterator->current_index;
	return HASH_KEY_IS_LONG;