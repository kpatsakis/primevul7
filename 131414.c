static int is_userinfo_valid(zend_string *str)
{
	const char *valid = "-._~!$&'()*+,;=:";
	const char *p = ZSTR_VAL(str);
	while (p - ZSTR_VAL(str) < ZSTR_LEN(str)) {
		if (isalpha(*p) || isdigit(*p) || strchr(valid, *p)) {
			p++;
		} else if (*p == '%' && p - ZSTR_VAL(str) <= ZSTR_LEN(str) - 3 && isdigit(*(p+1)) && isxdigit(*(p+2))) {
			p += 3;
		} else {
			return 0;
		}
	}
	return 1;
}