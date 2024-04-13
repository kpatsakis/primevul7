PHPAPI ZEND_COLD void php_error_docref1(const char *docref, const char *param1, int type, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	php_verror(docref, param1, type, format, args);
	va_end(args);
}