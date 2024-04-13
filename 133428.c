static void exif_iif_add_fmt(image_info_type *image_info, int section_index, char *name TSRMLS_DC, char *value, ...)
{
	char             *tmp;
	va_list 		 arglist;

	va_start(arglist, value);
	if (value) {
		vspprintf(&tmp, 0, value, arglist);
		exif_iif_add_str(image_info, section_index, name, tmp TSRMLS_CC);
		efree(tmp);
	}
	va_end(arglist);
}