static zend_bool ZEND_FASTCALL instanceof_interface(const zend_class_entry *instance_ce, const zend_class_entry *ce) /* {{{ */
{
	uint32_t i;

	if (instance_ce->num_interfaces) {
		ZEND_ASSERT(instance_ce->ce_flags & ZEND_ACC_RESOLVED_INTERFACES);
		for (i = 0; i < instance_ce->num_interfaces; i++) {
			if (instance_ce->interfaces[i] == ce) {
				return 1;
			}
		}
	}
	return instance_ce == ce;
}