PHP_METHOD(PharFileInfo, decompress)
{
	char *error;
	PHAR_ENTRY_OBJECT();

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (entry_obj->entry->is_dir) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, \
			"Phar entry is a directory, cannot set compression"); \
		return;
	}

	if ((entry_obj->entry->flags & PHAR_ENT_COMPRESSION_MASK) == 0) {
		RETURN_TRUE;
	}

	if (PHAR_G(readonly) && !entry_obj->entry->phar->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
			"Phar is readonly, cannot decompress");
		return;
	}

	if (entry_obj->entry->is_deleted) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
			"Cannot compress deleted file");
		return;
	}

	if ((entry_obj->entry->flags & PHAR_ENT_COMPRESSED_GZ) != 0 && !PHAR_G(has_zlib)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
			"Cannot decompress Gzip-compressed file, zlib extension is not enabled");
		return;
	}

	if ((entry_obj->entry->flags & PHAR_ENT_COMPRESSED_BZ2) != 0 && !PHAR_G(has_bz2)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
			"Cannot decompress Bzip2-compressed file, bz2 extension is not enabled");
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
	if (!entry_obj->entry->fp) {
		if (FAILURE == phar_open_archive_fp(entry_obj->entry->phar)) {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Cannot decompress entry \"%s\", phar error: Cannot open phar archive \"%s\" for reading", entry_obj->entry->filename, entry_obj->entry->phar->fname);
			return;
		}
		entry_obj->entry->fp_type = PHAR_FP;
	}

	entry_obj->entry->old_flags = entry_obj->entry->flags;
	entry_obj->entry->flags &= ~PHAR_ENT_COMPRESSION_MASK;
	entry_obj->entry->phar->is_modified = 1;
	entry_obj->entry->is_modified = 1;
	phar_flush(entry_obj->entry->phar, 0, 0, 0, &error);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
		efree(error);
	}
	RETURN_TRUE;
}