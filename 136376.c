tiff_document_class_init (TiffDocumentClass *klass)
{
	GObjectClass    *gobject_class = G_OBJECT_CLASS (klass);
	EvDocumentClass *ev_document_class = EV_DOCUMENT_CLASS (klass);

	gobject_class->finalize = tiff_document_finalize;

	ev_document_class->load = tiff_document_load;
	ev_document_class->save = tiff_document_save;
	ev_document_class->get_n_pages = tiff_document_get_n_pages;
	ev_document_class->get_page_size = tiff_document_get_page_size;
	ev_document_class->render = tiff_document_render;
	ev_document_class->get_thumbnail = tiff_document_get_thumbnail;
	ev_document_class->get_page_label = tiff_document_get_page_label;
}