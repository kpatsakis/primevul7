PHP_FUNCTION(set_time_limit)
{
	zend_long new_timeout;
	char *new_timeout_str;
	int new_timeout_strlen;
	zend_string *key;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &new_timeout) == FAILURE) {
		return;
	}

	new_timeout_strlen = (int)zend_spprintf(&new_timeout_str, 0, ZEND_LONG_FMT, new_timeout);

	key = zend_string_init("max_execution_time", sizeof("max_execution_time")-1, 0);
	if (zend_alter_ini_entry_chars_ex(key, new_timeout_str, new_timeout_strlen, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0) == SUCCESS) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
	zend_string_release(key);
	efree(new_timeout_str);
}