static void exif_process_APP12(image_info_type *ImageInfo, char *buffer, size_t length TSRMLS_DC)
{
	size_t l1, l2=0;

	if ((l1 = php_strnlen(buffer+2, length-2)) > 0) {
		exif_iif_add_tag(ImageInfo, SECTION_APP12, "Company", TAG_NONE, TAG_FMT_STRING, l1, buffer+2 TSRMLS_CC);
		if (length > 2+l1+1) {
			l2 = php_strnlen(buffer+2+l1+1, length-2-l1-1);
			exif_iif_add_tag(ImageInfo, SECTION_APP12, "Info", TAG_NONE, TAG_FMT_STRING, l2, buffer+2+l1+1 TSRMLS_CC);
		}
	}
#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Process section APP12 with l1=%d, l2=%d done", l1, l2);
#endif
}