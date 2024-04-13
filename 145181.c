static zval *php_get_configuration_directive_for_zend(zend_string *name)
{
	return cfg_get_entry_ex(name);
}