
static void date_object_free_storage_timezone(void *object TSRMLS_DC)
{
	php_timezone_obj *intern = (php_timezone_obj *)object;

	if (intern->type == TIMELIB_ZONETYPE_ABBR) {
		free(intern->tzi.z.abbr);
	}
	zend_object_std_dtor(&intern->std TSRMLS_CC);
	efree(object);