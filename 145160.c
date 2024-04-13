int php_register_extensions(zend_module_entry **ptr, int count)
{
	zend_module_entry **end = ptr + count;

	while (ptr < end) {
		if (*ptr) {
			if (zend_register_internal_module(*ptr)==NULL) {
				return FAILURE;
			}
		}
		ptr++;
	}
	return SUCCESS;
}