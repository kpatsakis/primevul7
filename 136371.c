tiff_document_save (EvDocument  *document,
		    const char  *uri,
		    GError     **error)
{		
	TiffDocument *tiff_document = TIFF_DOCUMENT (document);

	return ev_xfer_uri_simple (tiff_document->uri, uri, error); 
}