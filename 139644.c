e_mail_parser_parse_sync (EMailParser *parser,
                          CamelFolder *folder,
                          const gchar *message_uid,
                          CamelMimeMessage *message,
                          GCancellable *cancellable)
{
	EMailPartList *part_list;

	g_return_val_if_fail (E_IS_MAIL_PARSER (parser), NULL);
	g_return_val_if_fail (CAMEL_IS_MIME_MESSAGE (message), NULL);

	part_list = e_mail_part_list_new (message, message_uid, folder);

	mail_parser_run (parser, part_list, cancellable);

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

	return part_list;
}