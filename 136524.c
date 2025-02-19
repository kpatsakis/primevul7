static int phar_tar_writeheaders(void *pDest, void *argument TSRMLS_DC) /* {{{ */
{
	tar_header header;
	size_t pos;
	phar_entry_info *entry = (phar_entry_info *) pDest;
	struct _phar_pass_tar_info *fp = (struct _phar_pass_tar_info *)argument;
	char padding[512];

	if (entry->is_mounted) {
		return ZEND_HASH_APPLY_KEEP;
	}

	if (entry->is_deleted) {
		if (entry->fp_refcount <= 0) {
			return ZEND_HASH_APPLY_REMOVE;
		} else {
			/* we can't delete this in-memory until it is closed */
			return ZEND_HASH_APPLY_KEEP;
		}
	}

	phar_add_virtual_dirs(entry->phar, entry->filename, entry->filename_len TSRMLS_CC);
	memset((char *) &header, 0, sizeof(header));

	if (entry->filename_len > 100) {
		char *boundary;
		if (entry->filename_len > 256) {
			if (fp->error) {
				spprintf(fp->error, 4096, "tar-based phar \"%s\" cannot be created, filename \"%s\" is too long for tar file format", entry->phar->fname, entry->filename);
			}
			return ZEND_HASH_APPLY_STOP;
		}
		boundary = entry->filename + entry->filename_len - 101;
		while (*boundary && *boundary != '/') {
			++boundary;
		}
		if (!*boundary || ((boundary - entry->filename) > 155)) {
			if (fp->error) {
				spprintf(fp->error, 4096, "tar-based phar \"%s\" cannot be created, filename \"%s\" is too long for tar file format", entry->phar->fname, entry->filename);
			}
			return ZEND_HASH_APPLY_STOP;
		}
		memcpy(header.prefix, entry->filename, boundary - entry->filename);
		memcpy(header.name, boundary + 1, entry->filename_len - (boundary + 1 - entry->filename));
	} else {
		memcpy(header.name, entry->filename, entry->filename_len);
	}

	phar_tar_octal(header.mode, entry->flags & PHAR_ENT_PERM_MASK, sizeof(header.mode)-1);

	if (FAILURE == phar_tar_octal(header.size, entry->uncompressed_filesize, sizeof(header.size)-1)) {
		if (fp->error) {
			spprintf(fp->error, 4096, "tar-based phar \"%s\" cannot be created, filename \"%s\" is too large for tar file format", entry->phar->fname, entry->filename);
		}
		return ZEND_HASH_APPLY_STOP;
	}

	if (FAILURE == phar_tar_octal(header.mtime, entry->timestamp, sizeof(header.mtime)-1)) {
		if (fp->error) {
			spprintf(fp->error, 4096, "tar-based phar \"%s\" cannot be created, file modification time of file \"%s\" is too large for tar file format", entry->phar->fname, entry->filename);
		}
		return ZEND_HASH_APPLY_STOP;
	}

	/* calc checksum */
	header.typeflag = entry->tar_type;

	if (entry->link) {
		strncpy(header.linkname, entry->link, strlen(entry->link));
	}

	strncpy(header.magic, "ustar", sizeof("ustar")-1);
	strncpy(header.version, "00", sizeof("00")-1);
	strncpy(header.checksum, "        ", sizeof("        ")-1);
	entry->crc32 = phar_tar_checksum((char *)&header, sizeof(header));

	if (FAILURE == phar_tar_octal(header.checksum, entry->crc32, sizeof(header.checksum)-1)) {
		if (fp->error) {
			spprintf(fp->error, 4096, "tar-based phar \"%s\" cannot be created, checksum of file \"%s\" is too large for tar file format", entry->phar->fname, entry->filename);
		}
		return ZEND_HASH_APPLY_STOP;
	}

	/* write header */
	entry->header_offset = php_stream_tell(fp->new);

	if (sizeof(header) != php_stream_write(fp->new, (char *) &header, sizeof(header))) {
		if (fp->error) {
			spprintf(fp->error, 4096, "tar-based phar \"%s\" cannot be created, header for  file \"%s\" could not be written", entry->phar->fname, entry->filename);
		}
		return ZEND_HASH_APPLY_STOP;
	}

	pos = php_stream_tell(fp->new); /* save start of file within tar */

	/* write contents */
	if (entry->uncompressed_filesize) {
		if (FAILURE == phar_open_entry_fp(entry, fp->error, 0 TSRMLS_CC)) {
			return ZEND_HASH_APPLY_STOP;
		}

		if (-1 == phar_seek_efp(entry, 0, SEEK_SET, 0, 0 TSRMLS_CC)) {
			if (fp->error) {
				spprintf(fp->error, 4096, "tar-based phar \"%s\" cannot be created, contents of file \"%s\" could not be written, seek failed", entry->phar->fname, entry->filename);
			}
			return ZEND_HASH_APPLY_STOP;
		}

		if (SUCCESS != phar_stream_copy_to_stream(phar_get_efp(entry, 0 TSRMLS_CC), fp->new, entry->uncompressed_filesize, NULL)) {
			if (fp->error) {
				spprintf(fp->error, 4096, "tar-based phar \"%s\" cannot be created, contents of file \"%s\" could not be written", entry->phar->fname, entry->filename);
			}
			return ZEND_HASH_APPLY_STOP;
		}

		memset(padding, 0, 512);
		php_stream_write(fp->new, padding, ((entry->uncompressed_filesize +511)&~511) - entry->uncompressed_filesize);
	}

	if (!entry->is_modified && entry->fp_refcount) {
		/* open file pointers refer to this fp, do not free the stream */
		switch (entry->fp_type) {
			case PHAR_FP:
				fp->free_fp = 0;
				break;
			case PHAR_UFP:
				fp->free_ufp = 0;
			default:
				break;
		}
	}

	entry->is_modified = 0;

	if (entry->fp_type == PHAR_MOD && entry->fp != entry->phar->fp && entry->fp != entry->phar->ufp) {
		if (!entry->fp_refcount) {
			php_stream_close(entry->fp);
		}
		entry->fp = NULL;
	}

	entry->fp_type = PHAR_FP;

	/* note new location within tar */
	entry->offset = entry->offset_abs = pos;
	return ZEND_HASH_APPLY_KEEP;
}