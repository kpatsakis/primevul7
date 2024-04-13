ZEND_API zend_bool ZEND_FASTCALL instanceof_function_ex(const zend_class_entry *instance_ce, const zend_class_entry *ce, zend_bool is_interface) /* {{{ */
{
	if (is_interface) {
		ZEND_ASSERT(ce->ce_flags & ZEND_ACC_INTERFACE);
		return instanceof_interface(instance_ce, ce);
	} else {
		ZEND_ASSERT(!(ce->ce_flags & ZEND_ACC_INTERFACE));
		return instanceof_class(instance_ce, ce);
	}
}