static PHP_INI_MH(OnUpdateDisplayErrors)
{
	PG(display_errors) = (zend_bool) php_get_display_errors_mode(ZSTR_VAL(new_value), (int)ZSTR_LEN(new_value));

	return SUCCESS;
}