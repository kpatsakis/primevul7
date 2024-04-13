ZEND_API void ZEND_FASTCALL zend_str_tolower(char *str, size_t length) /* {{{ */
{
	register unsigned char *p = (unsigned char*)str;
	register unsigned char *end = p + length;

	while (p < end) {
		*p = zend_tolower_ascii(*p);
		p++;
	}
}