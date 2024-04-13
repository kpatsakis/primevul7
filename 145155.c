PHPAPI ZEND_COLD void php_error_docref0(const char *docref, int type, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	php_verror(docref, "", type, format, args);
	va_end(args);
}