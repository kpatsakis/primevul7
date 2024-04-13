tiff_document_get_page_size (EvDocument *document,
			     EvPage     *page,
			     double     *width,
			     double     *height)
{
	guint32 w, h;
	gfloat x_res, y_res;
	TiffDocument *tiff_document = TIFF_DOCUMENT (document);
	
	g_return_if_fail (TIFF_IS_DOCUMENT (document));
	g_return_if_fail (tiff_document->tiff != NULL);
	
	push_handlers ();
	if (TIFFSetDirectory (tiff_document->tiff, page->index) != 1) {
		pop_handlers ();
		return;
	}
	
	TIFFGetField (tiff_document->tiff, TIFFTAG_IMAGEWIDTH, &w);
	TIFFGetField (tiff_document->tiff, TIFFTAG_IMAGELENGTH, &h);
	tiff_document_get_resolution (tiff_document, &x_res, &y_res);
	h = h * (x_res / y_res);
	
	*width = w;
	*height = h;
	
	pop_handlers ();
}