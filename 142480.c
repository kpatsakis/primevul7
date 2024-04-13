SAPI_API int sapi_get_target_uid(uid_t *obj TSRMLS_DC)
{
	if (sapi_module.get_target_uid) {
		return sapi_module.get_target_uid(obj TSRMLS_CC);
	} else {
		return FAILURE;
	}
}