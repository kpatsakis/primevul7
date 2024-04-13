static PHP_INI_DISP(display_errors_mode)
{
	int mode, tmp_value_length, cgi_or_cli;
	char *tmp_value;

	if (type == ZEND_INI_DISPLAY_ORIG && ini_entry->modified) {
		tmp_value = (ini_entry->orig_value ? ZSTR_VAL(ini_entry->orig_value) : NULL );
		tmp_value_length = (int)(ini_entry->orig_value? ZSTR_LEN(ini_entry->orig_value) : 0);
	} else if (ini_entry->value) {
		tmp_value = ZSTR_VAL(ini_entry->value);
		tmp_value_length = (int)ZSTR_LEN(ini_entry->value);
	} else {
		tmp_value = NULL;
		tmp_value_length = 0;
	}

	mode = php_get_display_errors_mode(tmp_value, tmp_value_length);

	/* Display 'On' for other SAPIs instead of STDOUT or STDERR */
	cgi_or_cli = (!strcmp(sapi_module.name, "cli") || !strcmp(sapi_module.name, "cgi"));

	switch (mode) {
		case PHP_DISPLAY_ERRORS_STDERR:
			if (cgi_or_cli ) {
				PUTS("STDERR");
			} else {
				PUTS("On");
			}
			break;

		case PHP_DISPLAY_ERRORS_STDOUT:
			if (cgi_or_cli ) {
				PUTS("STDOUT");
			} else {
				PUTS("On");
			}
			break;

		default:
			PUTS("Off");
			break;
	}
}