void php_filter_int(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	zval *option_val;
	zend_long  min_range, max_range, option_flags;
	int   min_range_set, max_range_set;
	int   allow_octal = 0, allow_hex = 0;
	size_t	  len;
	int error = 0;
	zend_long  ctx_value;
	char *p;

	/* Parse options */
	FETCH_LONG_OPTION(min_range,    "min_range");
	FETCH_LONG_OPTION(max_range,    "max_range");
	option_flags = flags;

	len = Z_STRLEN_P(value);

	if (len == 0) {
		RETURN_VALIDATION_FAILED
	}

	if (option_flags & FILTER_FLAG_ALLOW_OCTAL) {
		allow_octal = 1;
	}

	if (option_flags & FILTER_FLAG_ALLOW_HEX) {
		allow_hex = 1;
	}

	/* Start the validating loop */
	p = Z_STRVAL_P(value);
	ctx_value = 0;

	PHP_FILTER_TRIM_DEFAULT(p, len);

	if (*p == '0') {
		p++; len--;
		if (allow_hex && (*p == 'x' || *p == 'X')) {
			p++; len--;
			if (php_filter_parse_hex(p, len, &ctx_value) < 0) {
				error = 1;
			}
		} else if (allow_octal) {
			if (php_filter_parse_octal(p, len, &ctx_value) < 0) {
				error = 1;
			}
		} else if (len != 0) {
			error = 1;
		}
	} else {
		if (php_filter_parse_int(p, len, &ctx_value) < 0) {
			error = 1;
		}
	}

	if (error > 0 || (min_range_set && (ctx_value < min_range)) || (max_range_set && (ctx_value > max_range))) {
		RETURN_VALIDATION_FAILED
	} else {
		zval_ptr_dtor(value);
		ZVAL_LONG(value, ctx_value);
		return;
	}
}