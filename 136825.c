PHP_METHOD(Phar, mungServer)
{
	zval *mungvalues, *data;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &mungvalues) == FAILURE) {
		return;
	}

	if (!zend_hash_num_elements(Z_ARRVAL_P(mungvalues))) {
		zend_throw_exception_ex(phar_ce_PharException, 0, "No values passed to Phar::mungServer(), expecting an array of any of these strings: PHP_SELF, REQUEST_URI, SCRIPT_FILENAME, SCRIPT_NAME");
		return;
	}

	if (zend_hash_num_elements(Z_ARRVAL_P(mungvalues)) > 4) {
		zend_throw_exception_ex(phar_ce_PharException, 0, "Too many values passed to Phar::mungServer(), expecting an array of any of these strings: PHP_SELF, REQUEST_URI, SCRIPT_FILENAME, SCRIPT_NAME");
		return;
	}

	phar_request_initialize();

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(mungvalues), data) {

		if (Z_TYPE_P(data) != IS_STRING) {
			zend_throw_exception_ex(phar_ce_PharException, 0, "Non-string value passed to Phar::mungServer(), expecting an array of any of these strings: PHP_SELF, REQUEST_URI, SCRIPT_FILENAME, SCRIPT_NAME");
			return;
		}

		if (Z_STRLEN_P(data) == sizeof("PHP_SELF")-1 && !strncmp(Z_STRVAL_P(data), "PHP_SELF", sizeof("PHP_SELF")-1)) {
			PHAR_G(phar_SERVER_mung_list) |= PHAR_MUNG_PHP_SELF;
		}

		if (Z_STRLEN_P(data) == sizeof("REQUEST_URI")-1) {
			if (!strncmp(Z_STRVAL_P(data), "REQUEST_URI", sizeof("REQUEST_URI")-1)) {
				PHAR_G(phar_SERVER_mung_list) |= PHAR_MUNG_REQUEST_URI;
			}
			if (!strncmp(Z_STRVAL_P(data), "SCRIPT_NAME", sizeof("SCRIPT_NAME")-1)) {
				PHAR_G(phar_SERVER_mung_list) |= PHAR_MUNG_SCRIPT_NAME;
			}
		}

		if (Z_STRLEN_P(data) == sizeof("SCRIPT_FILENAME")-1 && !strncmp(Z_STRVAL_P(data), "SCRIPT_FILENAME", sizeof("SCRIPT_FILENAME")-1)) {
			PHAR_G(phar_SERVER_mung_list) |= PHAR_MUNG_SCRIPT_FILENAME;
		}
	} ZEND_HASH_FOREACH_END();
}