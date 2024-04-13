ZEND_API zend_bool ZEND_FASTCALL instanceof_function(const zend_class_entry *instance_ce, const zend_class_entry *ce) /* {{{ */
{
	if (ce->ce_flags & ZEND_ACC_INTERFACE) {
		return instanceof_interface(instance_ce, ce);
	} else {
		return instanceof_class(instance_ce, ce);
	}
}