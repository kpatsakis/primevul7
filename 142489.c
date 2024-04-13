SAPI_API char *sapi_get_default_content_type(TSRMLS_D)
{
	uint len;

	return get_default_content_type(0, &len TSRMLS_CC);
}