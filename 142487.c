SAPI_API int sapi_get_target_gid(gid_t *obj TSRMLS_DC)
{
	if (sapi_module.get_target_gid) {
		return sapi_module.get_target_gid(obj TSRMLS_CC);
	} else {
		return FAILURE;
	}
}