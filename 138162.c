
static zend_object_value date_object_new_timezone(zend_class_entry *class_type TSRMLS_DC)
{
	return date_object_new_timezone_ex(class_type, NULL TSRMLS_CC);