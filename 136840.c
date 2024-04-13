PHP_METHOD(PharFileInfo, chmod)
{
	char *error;
	zend_long perms;
	PHAR_ENTRY_OBJECT();

	if (entry_obj->entry->is_temp_dir) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, \
			"Phar entry \"%s\" is a temporary directory (not an actual entry in the archive), cannot chmod", entry_obj->entry->filename); \
		return;
	}

	if (PHAR_G(readonly) && !entry_obj->entry->phar->is_data) {
		zend_throw_exception_ex(phar_ce_PharException, 0, "Cannot modify permissions for file \"%s\" in phar \"%s\", write operations are prohibited", entry_obj->entry->filename, entry_obj->entry->phar->fname);
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &perms) == FAILURE) {
		return;
	}

	if (entry_obj->entry->is_persistent) {
		phar_archive_data *phar = entry_obj->entry->phar;

		if (FAILURE == phar_copy_on_write(&phar)) {
			zend_throw_exception_ex(phar_ce_PharException, 0, "phar \"%s\" is persistent, unable to copy on write", phar->fname);
			return;
		}
		/* re-populate after copy-on-write */
		entry_obj->entry = zend_hash_str_find_ptr(&phar->manifest, entry_obj->entry->filename, entry_obj->entry->filename_len);
	}
	/* clear permissions */
	entry_obj->entry->flags &= ~PHAR_ENT_PERM_MASK;
	perms &= 0777;
	entry_obj->entry->flags |= perms;
	entry_obj->entry->old_flags = entry_obj->entry->flags;
	entry_obj->entry->phar->is_modified = 1;
	entry_obj->entry->is_modified = 1;

	/* hackish cache in php_stat needs to be cleared */
	/* if this code fails to work, check main/streams/streams.c, _php_stream_stat_path */
	if (BG(CurrentLStatFile)) {
		efree(BG(CurrentLStatFile));
	}

	if (BG(CurrentStatFile)) {
		efree(BG(CurrentStatFile));
	}

	BG(CurrentLStatFile) = NULL;
	BG(CurrentStatFile) = NULL;
	phar_flush(entry_obj->entry->phar, 0, 0, 0, &error);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
		efree(error);
	}
}