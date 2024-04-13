SAPI_API int sapi_register_post_entry(sapi_post_entry *post_entry TSRMLS_DC)
{
	if (SG(sapi_started) && EG(in_execution)) {
		return FAILURE;
	}
	return zend_hash_add(&SG(known_post_content_types),
			post_entry->content_type, post_entry->content_type_len+1,
			(void *) post_entry, sizeof(sapi_post_entry), NULL);
}