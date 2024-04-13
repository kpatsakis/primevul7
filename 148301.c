ZEND_API void multi_convert_to_long_ex(int argc, ...) /* {{{ */
{
	zval *arg;
	va_list ap;

	va_start(ap, argc);

	while (argc--) {
		arg = va_arg(ap, zval *);
		convert_to_long_ex(arg);
	}

	va_end(ap);
}