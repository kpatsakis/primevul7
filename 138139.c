
static void date_object_free_storage_interval(void *object TSRMLS_DC)
{
	php_interval_obj *intern = (php_interval_obj *)object;

	timelib_rel_time_dtor(intern->diff);
	zend_object_std_dtor(&intern->std TSRMLS_CC);
	efree(object);