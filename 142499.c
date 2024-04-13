SAPI_API int sapi_register_post_entries(sapi_post_entry *post_entries TSRMLS_DC)
{
	sapi_post_entry *p=post_entries;

	while (p->content_type) {
		if (sapi_register_post_entry(p TSRMLS_CC) == FAILURE) {
			return FAILURE;
		}
		p++;
	}
	return SUCCESS;
}