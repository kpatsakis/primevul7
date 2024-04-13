/* {{{ date_period_it_rewind */
static void date_period_it_rewind(zend_object_iterator *iter TSRMLS_DC)
{
	date_period_it   *iterator = (date_period_it *)iter;

	iterator->current_index = 0;
	if (iterator->object->current) {
		timelib_time_dtor(iterator->object->current);
	}
	iterator->object->current = timelib_time_clone(iterator->object->start);
	date_period_it_invalidate_current(iter TSRMLS_CC);