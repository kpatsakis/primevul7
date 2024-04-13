PHP_METHOD(Phar, getStub)
{
	size_t len;
	zend_string *buf;
	php_stream *fp;
	php_stream_filter *filter = NULL;
	phar_entry_info *stub;

	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (phar_obj->archive->is_tar || phar_obj->archive->is_zip) {

		if (NULL != (stub = zend_hash_str_find_ptr(&(phar_obj->archive->manifest), ".phar/stub.php", sizeof(".phar/stub.php")-1))) {
			if (phar_obj->archive->fp && !phar_obj->archive->is_brandnew && !(stub->flags & PHAR_ENT_COMPRESSION_MASK)) {
				fp = phar_obj->archive->fp;
			} else {
				if (!(fp = php_stream_open_wrapper(phar_obj->archive->fname, "rb", 0, NULL))) {
					zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0, "phar error: unable to open phar \"%s\"", phar_obj->archive->fname);
					return;
				}
				if (stub->flags & PHAR_ENT_COMPRESSION_MASK) {
					char *filter_name;

					if ((filter_name = phar_decompress_filter(stub, 0)) != NULL) {
						filter = php_stream_filter_create(filter_name, NULL, php_stream_is_persistent(fp));
					} else {
						filter = NULL;
					}
					if (!filter) {
						zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0, "phar error: unable to read stub of phar \"%s\" (cannot create %s filter)", phar_obj->archive->fname, phar_decompress_filter(stub, 1));
						return;
					}
					php_stream_filter_append(&fp->readfilters, filter);
				}
			}

			if (!fp)  {
				zend_throw_exception_ex(spl_ce_RuntimeException, 0,
					"Unable to read stub");
				return;
			}

			php_stream_seek(fp, stub->offset_abs, SEEK_SET);
			len = stub->uncompressed_filesize;
			goto carry_on;
		} else {
			RETURN_EMPTY_STRING();
		}
	}
	len = phar_obj->archive->halt_offset;

	if (phar_obj->archive->fp && !phar_obj->archive->is_brandnew) {
		fp = phar_obj->archive->fp;
	} else {
		fp = php_stream_open_wrapper(phar_obj->archive->fname, "rb", 0, NULL);
	}

	if (!fp)  {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0,
			"Unable to read stub");
		return;
	}

	php_stream_rewind(fp);
carry_on:
	buf = zend_string_alloc(len, 0);

	if (len != php_stream_read(fp, ZSTR_VAL(buf), len)) {
		if (fp != phar_obj->archive->fp) {
			php_stream_close(fp);
		}
		zend_throw_exception_ex(spl_ce_RuntimeException, 0,
			"Unable to read stub");
		zend_string_release(buf);
		return;
	}

	if (filter) {
		php_stream_filter_flush(filter, 1);
		php_stream_filter_remove(filter, 1);
	}

	if (fp != phar_obj->archive->fp) {
		php_stream_close(fp);
	}

	ZSTR_VAL(buf)[len] = '\0';
	ZSTR_LEN(buf) = len;
	RETVAL_STR(buf);
}