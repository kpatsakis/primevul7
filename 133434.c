PHP_FUNCTION(exif_thumbnail)
{
	zval *p_width = 0, *p_height = 0, *p_imagetype = 0;
	char *p_name;
	int p_name_len, ret, arg_c = ZEND_NUM_ARGS();
	image_info_type ImageInfo;

	memset(&ImageInfo, 0, sizeof(ImageInfo));

	if (arg_c!=1 && arg_c!=3 && arg_c!=4) {
		WRONG_PARAM_COUNT;
	}

	if (zend_parse_parameters(arg_c TSRMLS_CC, "p|z/z/z/", &p_name, &p_name_len, &p_width, &p_height, &p_imagetype) == FAILURE) {
		return;
	}

	ret = exif_read_file(&ImageInfo, p_name, 1, 0 TSRMLS_CC);
	if (ret==FALSE) {
		exif_discard_imageinfo(&ImageInfo);
		RETURN_FALSE;
	}

#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, &ImageInfo, E_NOTICE, "Thumbnail data %d %d %d, %d x %d", ImageInfo.Thumbnail.data, ImageInfo.Thumbnail.size, ImageInfo.Thumbnail.filetype, ImageInfo.Thumbnail.width, ImageInfo.Thumbnail.height);
#endif
	if (!ImageInfo.Thumbnail.data || !ImageInfo.Thumbnail.size) {
		exif_discard_imageinfo(&ImageInfo);
		RETURN_FALSE;
	}

#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, &ImageInfo, E_NOTICE, "Returning thumbnail(%d)", ImageInfo.Thumbnail.size);
#endif

	ZVAL_STRINGL(return_value, ImageInfo.Thumbnail.data, ImageInfo.Thumbnail.size, 1);
	if (arg_c >= 3) {
		if (!ImageInfo.Thumbnail.width || !ImageInfo.Thumbnail.height) {
			exif_scan_thumbnail(&ImageInfo TSRMLS_CC);
		}
		zval_dtor(p_width);
		zval_dtor(p_height);
		ZVAL_LONG(p_width,  ImageInfo.Thumbnail.width);
		ZVAL_LONG(p_height, ImageInfo.Thumbnail.height);
	}
	if (arg_c >= 4)	{
		zval_dtor(p_imagetype);
		ZVAL_LONG(p_imagetype, ImageInfo.Thumbnail.filetype);
	}

#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, &ImageInfo, E_NOTICE, "Discarding info");
#endif

	exif_discard_imageinfo(&ImageInfo);

#ifdef EXIF_DEBUG
	php_error_docref1(NULL TSRMLS_CC, p_name, E_NOTICE, "Done");
#endif
}