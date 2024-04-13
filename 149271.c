static union _zend_function *incomplete_class_get_method(zval **object, char *method, int method_len, const zend_literal *key TSRMLS_DC) /* {{{ */
{
	incomplete_class_message(*object, E_ERROR TSRMLS_CC);
	return NULL;
}