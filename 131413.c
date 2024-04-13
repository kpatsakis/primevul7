void php_filter_validate_mac(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	char *input = Z_STRVAL_P(value);
	size_t input_len = Z_STRLEN_P(value);
	int tokens, length, i, offset, exp_separator_set;
	size_t exp_separator_len;
	char separator;
	char *exp_separator;
	zend_long ret = 0;
	zval *option_val;

	FETCH_STRING_OPTION(exp_separator, "separator");

	if (exp_separator_set && exp_separator_len != 1) {
		php_error_docref(NULL, E_WARNING, "Separator must be exactly one character long");
		RETURN_VALIDATION_FAILED;
	}

	if (14 == input_len) {
		/* EUI-64 format: Four hexadecimal digits separated by dots. Less
		 * commonly used but valid nonetheless.
		 */
		tokens = 3;
		length = 4;
		separator = '.';
	} else if (17 == input_len && input[2] == '-') {
		/* IEEE 802 format: Six hexadecimal digits separated by hyphens. */
		tokens = 6;
		length = 2;
		separator = '-';
	} else if (17 == input_len && input[2] == ':') {
		/* IEEE 802 format: Six hexadecimal digits separated by colons. */
		tokens = 6;
		length = 2;
		separator = ':';
	} else {
		RETURN_VALIDATION_FAILED;
	}

	if (exp_separator_set && separator != exp_separator[0]) {
		RETURN_VALIDATION_FAILED;
	}

	/* Essentially what we now have is a set of tokens each consisting of
	 * a hexadecimal number followed by a separator character. (With the
	 * exception of the last token which does not have the separator.)
	 */
	for (i = 0; i < tokens; i++) {
		offset = i * (length + 1);

		if (i < tokens - 1 && input[offset + length] != separator) {
			/* The current token did not end with e.g. a "." */
			RETURN_VALIDATION_FAILED
		}
		if (php_filter_parse_hex(input + offset, length, &ret) < 0) {
			/* The current token is no valid hexadecimal digit */
			RETURN_VALIDATION_FAILED
		}
	}
}