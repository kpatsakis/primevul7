file_printable(char *buf, size_t bufsiz, const char *str, size_t slen)
{
	char *ptr, *eptr = buf + bufsiz - 1;
	const unsigned char *s = (const unsigned char *)str;
	const unsigned char *es = s + slen;

	for (ptr = buf;  ptr < eptr && s < es && *s; s++) {
		if (isprint(*s)) {
			*ptr++ = *s;
			continue;
		}
		if (ptr >= eptr - 3)
			break;
		*ptr++ = '\\';
		*ptr++ = ((CAST(unsigned int, *s) >> 6) & 7) + '0';
		*ptr++ = ((CAST(unsigned int, *s) >> 3) & 7) + '0';
		*ptr++ = ((CAST(unsigned int, *s) >> 0) & 7) + '0';
	}
	*ptr = '\0';
	return buf;
}