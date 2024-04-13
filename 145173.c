static void php_disable_functions(void)
{
	char *s = NULL, *e;

	if (!*(INI_STR("disable_functions"))) {
		return;
	}

	e = PG(disable_functions) = strdup(INI_STR("disable_functions"));
	if (e == NULL) {
		return;
	}
	while (*e) {
		switch (*e) {
			case ' ':
			case ',':
				if (s) {
					*e = '\0';
					zend_disable_function(s, e-s);
					s = NULL;
				}
				break;
			default:
				if (!s) {
					s = e;
				}
				break;
		}
		e++;
	}
	if (s) {
		zend_disable_function(s, e-s);
	}
}