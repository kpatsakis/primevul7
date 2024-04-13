tiff_document_finalize (GObject *object)
{
	TiffDocument *tiff_document = TIFF_DOCUMENT (object);

	if (tiff_document->tiff)
		TIFFClose (tiff_document->tiff);
	if (tiff_document->uri)
		g_free (tiff_document->uri);

	G_OBJECT_CLASS (tiff_document_parent_class)->finalize (object);
}