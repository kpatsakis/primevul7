PHP_METHOD(PharFileInfo, compress)
{
	zend_long method;
	char *error;
	PHAR_ENTRY_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &method) == FAILURE) {
		return;
	}

	if (entry_obj->entry->is_tar) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
			"Cannot compress with Gzip compression, not possible with tar-based phar archives");
		return;
	}

	if (entry_obj->entry->is_dir) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, \
			"Phar entry is a directory, cannot set compression"); \
		return;
	}

	if (PHAR_G(readonly) && !entry_obj->entry->phar->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
			"Phar is readonly, cannot change compression");
		return;
	}

	if (entry_obj->entry->is_deleted) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
			"Cannot compress deleted file");
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
	switch (method) {
		case PHAR_ENT_COMPRESSED_GZ:
			if (entry_obj->entry->flags & PHAR_ENT_COMPRESSED_GZ) {
				RETURN_TRUE;
			}

			if ((entry_obj->entry->flags & PHAR_ENT_COMPRESSED_BZ2) != 0) {
				if (!PHAR_G(has_bz2)) {
					zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
						"Cannot compress with gzip compression, file is already compressed with bzip2 compression and bz2 extension is not enabled, cannot decompress");
					return;
				}

				/* decompress this file indirectly */
				if (SUCCESS != phar_open_entry_fp(entry_obj->entry, &error, 1)) {
					zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
						"Phar error: Cannot decompress bzip2-compressed file \"%s\" in phar \"%s\" in order to compress with gzip: %s", entry_obj->entry->filename, entry_obj->entry->phar->fname, error);
					efree(error);
					return;
				}
			}

			if (!PHAR_G(has_zlib)) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
					"Cannot compress with gzip compression, zlib extension is not enabled");
				return;
			}

			entry_obj->entry->old_flags = entry_obj->entry->flags;
			entry_obj->entry->flags &= ~PHAR_ENT_COMPRESSION_MASK;
			entry_obj->entry->flags |= PHAR_ENT_COMPRESSED_GZ;
			break;
		case PHAR_ENT_COMPRESSED_BZ2:
			if (entry_obj->entry->flags & PHAR_ENT_COMPRESSED_BZ2) {
				RETURN_TRUE;
			}

			if ((entry_obj->entry->flags & PHAR_ENT_COMPRESSED_GZ) != 0) {
				if (!PHAR_G(has_zlib)) {
					zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
						"Cannot compress with bzip2 compression, file is already compressed with gzip compression and zlib extension is not enabled, cannot decompress");
					return;
				}

				/* decompress this file indirectly */
				if (SUCCESS != phar_open_entry_fp(entry_obj->entry, &error, 1)) {
					zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
						"Phar error: Cannot decompress gzip-compressed file \"%s\" in phar \"%s\" in order to compress with bzip2: %s", entry_obj->entry->filename, entry_obj->entry->phar->fname, error);
					efree(error);
					return;
				}
			}

			if (!PHAR_G(has_bz2)) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
					"Cannot compress with bzip2 compression, bz2 extension is not enabled");
				return;
			}
			entry_obj->entry->old_flags = entry_obj->entry->flags;
			entry_obj->entry->flags &= ~PHAR_ENT_COMPRESSION_MASK;
			entry_obj->entry->flags |= PHAR_ENT_COMPRESSED_BZ2;
			break;
		default:
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, \
				"Unknown compression type specified"); \
	}

	entry_obj->entry->phar->is_modified = 1;
	entry_obj->entry->is_modified = 1;
	phar_flush(entry_obj->entry->phar, 0, 0, 0, &error);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
		efree(error);
	}

	RETURN_TRUE;
}