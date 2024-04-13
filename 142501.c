SAPI_API int sapi_force_http_10(TSRMLS_D)
{
	if (sapi_module.force_http_10) {
		return sapi_module.force_http_10(TSRMLS_C);
	} else {
		return FAILURE;
	}
}