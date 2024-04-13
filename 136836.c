PHP_METHOD(PharFileInfo, delMetadata)
{
	char *error;

	PHAR_ENTRY_OBJECT();

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (PHAR_G(readonly) && !entry_obj->entry->phar->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Write operations disabled by the php.ini setting phar.readonly");
		return;
	}

	if (entry_obj->entry->is_temp_dir) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, \
			"Phar entry is a temporary directory (not an actual entry in the archive), cannot delete metadata"); \
		return;
	}

	if (Z_TYPE(entry_obj->entry->metadata) != IS_UNDEF) {
		if (entry_obj->entry->is_persistent) {
			phar_archive_data *phar = entry_obj->entry->phar;

			if (FAILURE == phar_copy_on_write(&phar)) {
				zend_throw_exception_ex(phar_ce_PharException, 0, "phar \"%s\" is persistent, unable to copy on write", phar->fname);
				return;
			}
			/* re-populate after copy-on-write */
			entry_obj->entry = zend_hash_str_find_ptr(&phar->manifest, entry_obj->entry->filename, entry_obj->entry->filename_len);
		}
		zval_ptr_dtor(&entry_obj->entry->metadata);
		ZVAL_UNDEF(&entry_obj->entry->metadata);
		entry_obj->entry->is_modified = 1;
		entry_obj->entry->phar->is_modified = 1;

		phar_flush(entry_obj->entry->phar, 0, 0, 0, &error);

		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
			efree(error);
			RETURN_FALSE;
		} else {
			RETURN_TRUE;
		}

	} else {
		RETURN_TRUE;
	}
}