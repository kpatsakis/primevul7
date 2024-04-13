/* {{{ date_period_it_has_more */
static int date_period_it_has_more(zend_object_iterator *iter TSRMLS_DC)
{
	date_period_it *iterator = (date_period_it *)iter;
	php_period_obj *object   = iterator->object;
	timelib_time   *it_time = object->current;

	/* apply modification if it's not the first iteration */
	if (!object->include_start_date || iterator->current_index > 0) {
		it_time->have_relative = 1;
		it_time->relative = *object->interval;
		it_time->sse_uptodate = 0;
		timelib_update_ts(it_time, NULL);
		timelib_update_from_sse(it_time);
	}

	if (object->end) {
		return object->current->sse < object->end->sse ? SUCCESS : FAILURE;
	} else {
		return (iterator->current_index < object->recurrences) ? SUCCESS : FAILURE;
	}