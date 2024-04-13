tiff_document_file_exporter_end (EvFileExporter *exporter)
{
	TiffDocument *document = TIFF_DOCUMENT (exporter);

	if (document->ps_export_ctx == NULL)
		return;
	tiff2ps_context_finalize(document->ps_export_ctx);
}