static zend_object_value spl_array_object_new(zend_class_entry *class_type TSRMLS_DC)
{
	spl_array_object *tmp;
	return spl_array_object_new_ex(class_type, &tmp, NULL, 0 TSRMLS_CC);
}