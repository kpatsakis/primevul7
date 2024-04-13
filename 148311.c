ZEND_API char* ZEND_FASTCALL zend_str_tolower_copy(char *dest, const char *source, size_t length) /* {{{ */
{
	register unsigned char *str = (unsigned char*)source;
	register unsigned char *result = (unsigned char*)dest;
	register unsigned char *end = str + length;

	while (str < end) {
		*result++ = zend_tolower_ascii(*str++);
	}
	*result = '\0';

	return dest;
}