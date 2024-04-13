tiff_document_get_resolution (TiffDocument *tiff_document,
			      gfloat       *x_res,
			      gfloat       *y_res)
{
	gfloat x = 0.0;
	gfloat y = 0.0;
	gushort unit;

	if (TIFFGetField (tiff_document->tiff, TIFFTAG_XRESOLUTION, &x) &&
	    TIFFGetField (tiff_document->tiff, TIFFTAG_YRESOLUTION, &y)) {
		if (TIFFGetFieldDefaulted (tiff_document->tiff, TIFFTAG_RESOLUTIONUNIT, &unit)) {
			if (unit == RESUNIT_CENTIMETER) {
				x *= 2.54;
				y *= 2.54;
			}
		}
	}

	/* Handle 0 values: some software set TIFF resolution as `0 , 0` see bug #646414 */
	*x_res = x > 0 ? x : 72.0;
	*y_res = y > 0 ? y : 72.0;
}