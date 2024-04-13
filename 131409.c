void php_filter_float(PHP_INPUT_FILTER_PARAM_DECL) /* {{{ */
{
	size_t len;
	char *str, *end;
	char *num, *p;
	zval *option_val;
	char *decimal;
	int decimal_set;
	size_t decimal_len;
	char dec_sep = '.';
	char *thousand;
	int thousand_set;
	size_t thousand_len;
	char *tsd_sep;

	zend_long lval;
	double dval;

	int first, n;

	len = Z_STRLEN_P(value);
	str = Z_STRVAL_P(value);

	PHP_FILTER_TRIM_DEFAULT(str, len);
	end = str + len;

	FETCH_STRING_OPTION(decimal, "decimal");

	if (decimal_set) {
		if (decimal_len != 1) {
			php_error_docref(NULL, E_WARNING, "decimal separator must be one char");
			RETURN_VALIDATION_FAILED
		} else {
			dec_sep = *decimal;
		}
	}

	FETCH_STRING_OPTION(thousand, "thousand");

	if (thousand_set) {
		if (thousand_len < 1) {
			php_error_docref(NULL, E_WARNING, "thousand separator must be at least one char");
			RETURN_VALIDATION_FAILED
		} else {
			tsd_sep = thousand;
		}
	} else {
		tsd_sep = "',.";
	}

	num = p = emalloc(len+1);
	if (str < end && (*str == '+' || *str == '-')) {
		*p++ = *str++;
	}
	first = 1;
	while (1) {
		n = 0;
		while (str < end && *str >= '0' && *str <= '9') {
			++n;
			*p++ = *str++;
		}
		if (str == end || *str == dec_sep || *str == 'e' || *str == 'E') {
			if (!first && n != 3) {
				goto error;
			}
			if (*str == dec_sep) {
				*p++ = '.';
				str++;
				while (str < end && *str >= '0' && *str <= '9') {
					*p++ = *str++;
				}
			}
			if (*str == 'e' || *str == 'E') {
				*p++ = *str++;
				if (str < end && (*str == '+' || *str == '-')) {
					*p++ = *str++;
				}
				while (str < end && *str >= '0' && *str <= '9') {
					*p++ = *str++;
				}
			}
			break;
		}
		if ((flags & FILTER_FLAG_ALLOW_THOUSAND) && strchr(tsd_sep, *str)) {
			if (first?(n < 1 || n > 3):(n != 3)) {
				goto error;
			}
			first = 0;
			str++;
		} else {
			goto error;
		}
	}
	if (str != end) {
		goto error;
	}
	*p = 0;

	switch (is_numeric_string(num, p - num, &lval, &dval, 0)) {
		case IS_LONG:
			zval_ptr_dtor(value);
			ZVAL_DOUBLE(value, (double)lval);
			break;
		case IS_DOUBLE:
			if ((!dval && p - num > 1 && strpbrk(num, "123456789")) || !zend_finite(dval)) {
				goto error;
			}
			zval_ptr_dtor(value);
			ZVAL_DOUBLE(value, dval);
			break;
		default:
error:
			efree(num);
			RETURN_VALIDATION_FAILED
	}
	efree(num);
}