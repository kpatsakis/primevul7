static void exif_process_TIFF_in_JPEG(image_info_type *ImageInfo, char *CharBuf, size_t length, size_t displacement TSRMLS_DC)
{
	unsigned exif_value_2a, offset_of_ifd;

	/* set the thumbnail stuff to nothing so we can test to see if they get set up */
	if (memcmp(CharBuf, "II", 2) == 0) {
		ImageInfo->motorola_intel = 0;
	} else if (memcmp(CharBuf, "MM", 2) == 0) {
		ImageInfo->motorola_intel = 1;
	} else {
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Invalid TIFF alignment marker");
		return;
	}

	/* Check the next two values for correctness. */
	if (length < 8) {
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Invalid TIFF start (1)");
		return;
	}
	exif_value_2a = php_ifd_get16u(CharBuf+2, ImageInfo->motorola_intel);
	offset_of_ifd = php_ifd_get32u(CharBuf+4, ImageInfo->motorola_intel);
	if (exif_value_2a != 0x2a || offset_of_ifd < 0x08) {
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Invalid TIFF start (1)");
		return;
	}
	if (offset_of_ifd > length) {
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Invalid IFD start");
		return;
	}

	ImageInfo->sections_found |= FOUND_IFD0;
	/* First directory starts at offset 8. Offsets starts at 0. */
	exif_process_IFD_in_JPEG(ImageInfo, CharBuf+offset_of_ifd, CharBuf, length/*-14*/, displacement, SECTION_IFD0 TSRMLS_CC);

#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Process TIFF in JPEG done");
#endif

	/* Compute the CCD width, in milimeters. */
	if (ImageInfo->FocalplaneXRes != 0) {
		ImageInfo->CCDWidth = (float)(ImageInfo->ExifImageWidth * ImageInfo->FocalplaneUnits / ImageInfo->FocalplaneXRes);
	}
}