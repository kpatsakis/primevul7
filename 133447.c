static void exif_process_CME (image_info_type *image_info, char *value, size_t length TSRMLS_DC)
{
	if (length>3) {
		switch(value[2]) {
			case 0:
				exif_iif_add_tag(image_info, SECTION_COMMENT, "Comment", TAG_COMPUTED_VALUE, TAG_FMT_UNDEFINED, length, value TSRMLS_CC);
				break;
			case 1:
				exif_iif_add_tag(image_info, SECTION_COMMENT, "Comment", TAG_COMPUTED_VALUE, TAG_FMT_STRING, length, value);
				break;
			default:
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Undefined JPEG2000 comment encoding");
				break;
		}
	} else {
		exif_iif_add_tag(image_info, SECTION_COMMENT, "Comment", TAG_COMPUTED_VALUE, TAG_FMT_UNDEFINED, 0, NULL);
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "JPEG2000 comment section too small");
	}
}