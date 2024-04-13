PHP_METHOD(Phar, getMetadata)
{
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (Z_TYPE(phar_obj->archive->metadata) != IS_UNDEF) {
		if (phar_obj->archive->is_persistent) {
			char *buf = estrndup((char *) Z_PTR(phar_obj->archive->metadata), phar_obj->archive->metadata_len);
			/* assume success, we would have failed before */
			phar_parse_metadata(&buf, return_value, phar_obj->archive->metadata_len);
			efree(buf);
		} else {
			ZVAL_COPY(return_value, &phar_obj->archive->metadata);
		}
	}
}