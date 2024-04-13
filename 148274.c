ZEND_API char* ZEND_FASTCALL zend_str_tolower_dup_ex(const char *source, size_t length) /* {{{ */
{
	register const unsigned char *p = (const unsigned char*)source;
	register const unsigned char *end = p + length;

	while (p < end) {
		if (*p != zend_tolower_ascii(*p)) {
			char *res = (char*)emalloc(length + 1);
			register unsigned char *r;

			if (p != (const unsigned char*)source) {
				memcpy(res, source, p - (const unsigned char*)source);
			}
			r = (unsigned char*)p + (res - source);
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
	return NULL;
}