static zend_object_value php_create_incomplete_object(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object *object;
	zend_object_value value;
	
	value = zend_objects_new(&object, class_type TSRMLS_CC);
	value.handlers = &php_incomplete_object_handlers;
	
	object_properties_init(object, class_type);
	
	return value;
}