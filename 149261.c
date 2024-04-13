static void incomplete_class_unset_property(zval *object, zval *member, const zend_literal *key TSRMLS_DC) /* {{{ */
{
	incomplete_class_message(object, E_NOTICE TSRMLS_CC);
}