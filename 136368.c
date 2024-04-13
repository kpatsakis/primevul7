tiff_document_load (EvDocument  *document,
		    const char  *uri,
		    GError     **error)
{
	TiffDocument *tiff_document = TIFF_DOCUMENT (document);
	gchar *filename;
	TIFF *tiff;
	
	filename = g_filename_from_uri (uri, NULL, error);
	if (!filename)
		return FALSE;
	
	push_handlers ();

#ifdef G_OS_WIN32
{
	wchar_t *wfilename = g_utf8_to_utf16 (filename, -1, NULL, NULL, error);
	if (wfilename == NULL) {
		return FALSE;
	}

	tiff = TIFFOpenW (wfilename, "r");

	g_free (wfilename);
}
#else
	tiff = TIFFOpen (filename, "r");
#endif
	if (tiff) {
		guint32 w, h;
		
		/* FIXME: unused data? why bother here */
		TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &w);
		TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &h);
	}
	
	if (!tiff) {
		pop_handlers ();

    		g_set_error_literal (error,
				     EV_DOCUMENT_ERROR,
				     EV_DOCUMENT_ERROR_INVALID,
				     _("Invalid document"));

		g_free (filename);
		return FALSE;
	}
	
	tiff_document->tiff = tiff;
	g_free (tiff_document->uri);
	g_free (filename);
	tiff_document->uri = g_strdup (uri);
	
	pop_handlers ();
	return TRUE;
}