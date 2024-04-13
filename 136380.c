tiff_document_document_file_exporter_iface_init (EvFileExporterInterface *iface)
{
	iface->begin = tiff_document_file_exporter_begin;
	iface->do_page = tiff_document_file_exporter_do_page;
	iface->end = tiff_document_file_exporter_end;
	iface->get_capabilities = tiff_document_file_exporter_get_capabilities;
}