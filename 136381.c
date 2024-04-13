tiff_document_get_n_pages (EvDocument  *document)
{
	TiffDocument *tiff_document = TIFF_DOCUMENT (document);
	
	g_return_val_if_fail (TIFF_IS_DOCUMENT (document), 0);
	g_return_val_if_fail (tiff_document->tiff != NULL, 0);
	
	if (tiff_document->n_pages == -1) {
		push_handlers ();
		tiff_document->n_pages = 0;
		
		do {
			tiff_document->n_pages ++;
		}
		while (TIFFReadDirectory (tiff_document->tiff));
		pop_handlers ();
	}

	return tiff_document->n_pages;
}