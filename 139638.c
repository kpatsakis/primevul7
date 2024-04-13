e_mail_parser_parse (EMailParser *parser,
                     CamelFolder *folder,
                     const gchar *message_uid,
                     CamelMimeMessage *message,
                     GAsyncReadyCallback callback,
                     GCancellable *cancellable,
                     gpointer user_data)
{
	GSimpleAsyncResult *simple;
	EMailPartList *part_list;

	g_return_if_fail (E_IS_MAIL_PARSER (parser));
	g_return_if_fail (CAMEL_IS_MIME_MESSAGE (message));

	part_list = e_mail_part_list_new (message, message_uid, folder);

	simple = g_simple_async_result_new (
		G_OBJECT (parser), callback,
		user_data, e_mail_parser_parse);

	g_simple_async_result_set_check_cancellable (simple, cancellable);

	g_simple_async_result_set_op_res_gpointer (
		simple, part_list, (GDestroyNotify) g_object_unref);

	g_simple_async_result_run_in_thread (
		simple, mail_parser_parse_thread,
		G_PRIORITY_DEFAULT, cancellable);

	g_object_unref (simple);
}