e_mail_parser_parse_finish (EMailParser *parser,
                            GAsyncResult *result,
                            GError **error)
{
	GSimpleAsyncResult *simple;
	EMailPartList *part_list;

	g_return_val_if_fail (
		g_simple_async_result_is_valid (
		result, G_OBJECT (parser), e_mail_parser_parse), NULL);

	simple = G_SIMPLE_ASYNC_RESULT (result);
	part_list = g_simple_async_result_get_op_res_gpointer (simple);

	if (camel_debug_start ("emformat:parser")) {
		GQueue queue = G_QUEUE_INIT;

		printf (
			"%s finished with EMailPartList:\n",
			G_OBJECT_TYPE_NAME (parser));

		e_mail_part_list_queue_parts (part_list, NULL, &queue);

		while (!g_queue_is_empty (&queue)) {
			EMailPart *part;

			part = g_queue_pop_head (&queue);

			printf (
				"	id: %s | cid: %s | mime_type: %s | "
				"is_hidden: %d | is_attachment: %d\n",
				e_mail_part_get_id (part),
				e_mail_part_get_cid (part),
				e_mail_part_get_mime_type (part),
				part->is_hidden ? 1 : 0,
				e_mail_part_get_is_attachment (part) ? 1 : 0);

			g_object_unref (part);
		}

		camel_debug_end ();
	}

	return g_object_ref (part_list);
}