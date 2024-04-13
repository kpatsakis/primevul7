SAPI_API struct stat *sapi_get_stat(TSRMLS_D)
{
	if (sapi_module.get_stat) {
		return sapi_module.get_stat(TSRMLS_C);
	} else {
		if (!SG(request_info).path_translated || (VCWD_STAT(SG(request_info).path_translated, &SG(global_stat)) == -1)) {
			return NULL;
		}
		return &SG(global_stat);
	}
}