SAPI_API void sapi_unregister_post_entry(sapi_post_entry *post_entry TSRMLS_DC)
{
	if (SG(sapi_started) && EG(in_execution)) {
		return;
	}
	zend_hash_del(&SG(known_post_content_types), post_entry->content_type,
			post_entry->content_type_len+1);
}