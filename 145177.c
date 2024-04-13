static int php_register_extensions_bc(zend_module_entry *ptr, int count)
{
	while (count--) {
		if (zend_register_internal_module(ptr++) == NULL) {
			return FAILURE;
 		}
	}
	return SUCCESS;
}