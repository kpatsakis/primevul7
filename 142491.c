SAPI_API int sapi_register_default_post_reader(void (*default_post_reader)(TSRMLS_D) TSRMLS_DC)
{
	if (SG(sapi_started) && EG(in_execution)) {
		return FAILURE;
	}
	sapi_module.default_post_reader = default_post_reader;
	return SUCCESS;
}