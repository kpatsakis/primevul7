tiff_document_file_exporter_begin (EvFileExporter        *exporter,
				   EvFileExporterContext *fc)
{
	TiffDocument *document = TIFF_DOCUMENT (exporter);

	document->ps_export_ctx = tiff2ps_context_new(fc->filename);
}