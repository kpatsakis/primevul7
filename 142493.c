static inline char *get_default_content_type(uint prefix_len, uint *len TSRMLS_DC)
{
	char *mimetype, *charset, *content_type;
	uint mimetype_len, charset_len;

	if (SG(default_mimetype)) {
		mimetype = SG(default_mimetype);
		mimetype_len = strlen(SG(default_mimetype));
	} else {
		mimetype = SAPI_DEFAULT_MIMETYPE;
		mimetype_len = sizeof(SAPI_DEFAULT_MIMETYPE) - 1;
	}
	if (SG(default_charset)) {
		charset = SG(default_charset);
		charset_len = strlen(SG(default_charset));
	} else {
		charset = SAPI_DEFAULT_CHARSET;
		charset_len = sizeof(SAPI_DEFAULT_CHARSET) - 1;
	}

	if (*charset && strncasecmp(mimetype, "text/", 5) == 0) {
		char *p;

		*len = prefix_len + mimetype_len + sizeof("; charset=") - 1 + charset_len;
		content_type = (char*)emalloc(*len + 1);
		p = content_type + prefix_len;
		memcpy(p, mimetype, mimetype_len);
		p += mimetype_len;
		memcpy(p, "; charset=", sizeof("; charset=") - 1);
		p += sizeof("; charset=") - 1;
		memcpy(p, charset, charset_len + 1);
	} else {
		*len = prefix_len + mimetype_len;
		content_type = (char*)emalloc(*len + 1);
		memcpy(content_type + prefix_len, mimetype, mimetype_len + 1);
	}
	return content_type;
}