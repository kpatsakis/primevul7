PHP_METHOD(PharFileInfo, getMetadata)
{
	PHAR_ENTRY_OBJECT();

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (Z_TYPE(entry_obj->entry->metadata) != IS_UNDEF) {
		if (entry_obj->entry->is_persistent) {
			char *buf = estrndup((char *) Z_PTR(entry_obj->entry->metadata), entry_obj->entry->metadata_len);
			/* assume success, we would have failed before */
			phar_parse_metadata(&buf, return_value, entry_obj->entry->metadata_len);
			efree(buf);
		} else {
			ZVAL_COPY(return_value, &entry_obj->entry->metadata);
		}
	}
}