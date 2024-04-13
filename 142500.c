SAPI_API int sapi_get_fd(int *fd TSRMLS_DC)
{
	if (sapi_module.get_fd) {
		return sapi_module.get_fd(fd TSRMLS_CC);
	} else {
		return FAILURE;
	}
}