PHPAPI size_t php_printf(const char *format, ...)
{
	va_list args;
	size_t ret;
	char *buffer;
	size_t size;

	va_start(args, format);
	size = vspprintf(&buffer, 0, format, args);
	ret = PHPWRITE(buffer, size);
	efree(buffer);
	va_end(args);

	return ret;
}