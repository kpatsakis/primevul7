static int incomplete_class_has_property(zval *object, zval *member, int check_empty, const zend_literal *key TSRMLS_DC) /* {{{ */
{
	incomplete_class_message(object, E_NOTICE TSRMLS_CC);
	return 0;
}