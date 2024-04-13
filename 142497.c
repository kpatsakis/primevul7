SAPI_API size_t sapi_apply_default_charset(char **mimetype, size_t len TSRMLS_DC)
{
	char *charset, *newtype;
	size_t newlen;
	charset = SG(default_charset) ? SG(default_charset) : SAPI_DEFAULT_CHARSET;

	if (*mimetype != NULL) {
		if (*charset && strncmp(*mimetype, "text/", 5) == 0 && strstr(*mimetype, "charset=") == NULL) {
			newlen = len + (sizeof(";charset=")-1) + strlen(charset);
			newtype = emalloc(newlen + 1);
	 		PHP_STRLCPY(newtype, *mimetype, newlen + 1, len);
			strlcat(newtype, ";charset=", newlen + 1);
			strlcat(newtype, charset, newlen + 1);
			efree(*mimetype);
			*mimetype = newtype;
			return newlen;
		}
	}
	return 0;
}