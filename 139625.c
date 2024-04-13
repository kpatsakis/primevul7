mail_parser_parse_thread (GSimpleAsyncResult *simple,
                          GObject *source_object,
                          GCancellable *cancellable)
{
	EMailPartList *part_list;

	part_list = g_simple_async_result_get_op_res_gpointer (simple);

	mail_parser_run (
		E_MAIL_PARSER (source_object),
		part_list, cancellable);
}