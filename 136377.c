tiff_document_file_exporter_do_page (EvFileExporter *exporter, EvRenderContext *rc)
{
	TiffDocument *document = TIFF_DOCUMENT (exporter);

	if (document->ps_export_ctx == NULL)
		return;
	if (TIFFSetDirectory (document->tiff, rc->page->index) != 1)
		return;
	tiff2ps_process_page (document->ps_export_ctx, document->tiff,
			      0, 0, 0, 0, 0);
}