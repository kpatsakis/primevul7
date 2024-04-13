static zval *incomplete_class_get_property(zval *object, zval *member, int type, const zend_literal *key TSRMLS_DC) /* {{{ */
{
	incomplete_class_message(object, E_NOTICE TSRMLS_CC);
	
	if (type == BP_VAR_W || type == BP_VAR_RW) {
		return EG(error_zval_ptr);
	} else {
		return EG(uninitialized_zval_ptr);
	}
}