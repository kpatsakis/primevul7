
static void date_object_free_storage_period(void *object TSRMLS_DC)
{
	php_period_obj *intern = (php_period_obj *)object;

	if (intern->start) {
		timelib_time_dtor(intern->start);
	}

	if (intern->current) {
		timelib_time_dtor(intern->current);
	}

	if (intern->end) {
		timelib_time_dtor(intern->end);
	}

	timelib_rel_time_dtor(intern->interval);
	zend_object_std_dtor(&intern->std TSRMLS_CC);
	efree(object);