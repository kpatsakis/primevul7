tiff_document_get_page_label (EvDocument *document,
			      EvPage     *page)
{
	TiffDocument *tiff_document = TIFF_DOCUMENT (document);
	static gchar *label;

	if (TIFFGetField (tiff_document->tiff, TIFFTAG_PAGENAME, &label) &&
	    g_utf8_validate (label, -1, NULL)) {
		return g_strdup (label);
	}

	return NULL;
}