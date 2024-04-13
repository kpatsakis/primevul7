
static inline zend_object_value date_object_new_interval_ex(zend_class_entry *class_type, php_interval_obj **ptr TSRMLS_DC)
{
	php_interval_obj *intern;
	zend_object_value retval;

	intern = emalloc(sizeof(php_interval_obj));
	memset(intern, 0, sizeof(php_interval_obj));
	if (ptr) {
		*ptr = intern;
	}

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	object_properties_init(&intern->std, class_type);
	
	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) date_object_free_storage_interval, NULL TSRMLS_CC);
	retval.handlers = &date_object_handlers_interval;
	
	return retval;