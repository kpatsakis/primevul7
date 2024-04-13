static zend_always_inline zend_bool instanceof_class(const zend_class_entry *instance_ce, const zend_class_entry *ce) /* {{{ */
{
	do {
		if (instance_ce == ce) {
			return 1;
		}
		instance_ce = instance_ce->parent;
	} while (instance_ce);
	return 0;
}