static void incomplete_class_message(zval *object, int error_type TSRMLS_DC)
{
	char *class_name;
	zend_bool class_name_alloced = 1;

	class_name = php_lookup_class_name(object, NULL);
	
	if (!class_name) {
		class_name_alloced = 0;
		class_name = "unknown";
	}
	
	php_error_docref(NULL TSRMLS_CC, error_type, INCOMPLETE_CLASS_MSG, class_name);

	if (class_name_alloced) {
		efree(class_name);
	}
}