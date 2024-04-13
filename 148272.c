ZEND_API zend_string* ZEND_FASTCALL zend_string_tolower_ex(zend_string *str, int persistent) /* {{{ */
{
	register unsigned char *p = (unsigned char*)ZSTR_VAL(str);
	register unsigned char *end = p + ZSTR_LEN(str);

	while (p < end) {
		if (*p != zend_tolower_ascii(*p)) {
			zend_string *res = zend_string_alloc(ZSTR_LEN(str), persistent);
			register unsigned char *r;

			if (p != (unsigned char*)ZSTR_VAL(str)) {
				memcpy(ZSTR_VAL(res), ZSTR_VAL(str), p - (unsigned char*)ZSTR_VAL(str));
			}
			r = p + (ZSTR_VAL(res) - ZSTR_VAL(str));
			while (p < end) {
				*r = zend_tolower_ascii(*p);
				p++;
				r++;
			}
			*r = '\0';
			return res;
		}
		p++;
	}
	return zend_string_copy(str);
}