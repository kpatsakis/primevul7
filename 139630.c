e_mail_parser_wrap_as_attachment (EMailParser *parser,
                                  CamelMimePart *part,
                                  GString *part_id,
                                  GQueue *parts_queue)
{
	EMailPartAttachment *empa;
	EAttachment *attachment;
	EMailPart *first_part;
	const gchar *snoop_mime_type;
	GQueue *extensions;
	CamelContentType *ct;
	gchar *mime_type;
	CamelDataWrapper *dw;
	GByteArray *ba;
	gsize size;
	gint part_id_len;

	ct = camel_mime_part_get_content_type (part);
	extensions = NULL;
	snoop_mime_type = NULL;
	if (ct) {
		EMailExtensionRegistry *reg;
		mime_type = camel_content_type_simple (ct);

		reg = e_mail_parser_get_extension_registry (parser);
		extensions = e_mail_extension_registry_get_for_mime_type (
			reg, mime_type);

		if (camel_content_type_is (ct, "text", "*") ||
		    camel_content_type_is (ct, "message", "*"))
			snoop_mime_type = mime_type;
		else
			g_free (mime_type);
	}

	if (!snoop_mime_type)
		snoop_mime_type = e_mail_part_snoop_type (part);

	if (!extensions) {
		EMailExtensionRegistry *reg;

		reg = e_mail_parser_get_extension_registry (parser);
		extensions = e_mail_extension_registry_get_for_mime_type (
			reg, snoop_mime_type);

		if (!extensions) {
			extensions = e_mail_extension_registry_get_fallback (
				reg, snoop_mime_type);
		}
	}

	part_id_len = part_id->len;
	g_string_append (part_id, ".attachment");

	empa = e_mail_part_attachment_new (part, part_id->str);
	empa->shown = extensions && (!g_queue_is_empty (extensions) &&
		e_mail_part_is_inline (part, extensions));
	empa->snoop_mime_type = snoop_mime_type;

	first_part = g_queue_peek_head (parts_queue);
	if (first_part != NULL && !E_IS_MAIL_PART_ATTACHMENT (first_part)) {
		const gchar *id = e_mail_part_get_id (first_part);
		empa->part_id_with_attachment = g_strdup (id);
		first_part->is_hidden = TRUE;
	}

	attachment = e_mail_part_attachment_ref_attachment (empa);

	e_attachment_set_initially_shown (attachment, empa->shown);
	e_attachment_set_can_show (
		attachment,
		extensions && !g_queue_is_empty (extensions));

	/* Try to guess size of the attachments */
	dw = camel_medium_get_content (CAMEL_MEDIUM (part));
	ba = camel_data_wrapper_get_byte_array (dw);
	if (ba) {
		size = ba->len;

		if (camel_mime_part_get_encoding (part) == CAMEL_TRANSFER_ENCODING_BASE64)
			size = size / 1.37;
	} else {
		size = 0;
	}

	/* e_attachment_load_async must be called from main thread */
	/* Prioritize ahead of GTK+ redraws. */
	g_idle_add_full (
		G_PRIORITY_HIGH_IDLE,
		(GSourceFunc) load_attachment_idle,
		g_object_ref (attachment),
		NULL);

	if (size != 0) {
		GFileInfo *file_info;

		file_info = e_attachment_ref_file_info (attachment);

		if (file_info == NULL) {
			file_info = g_file_info_new ();
			g_file_info_set_content_type (
				file_info, empa->snoop_mime_type);
		}

		g_file_info_set_size (file_info, size);
		e_attachment_set_file_info (attachment, file_info);

		g_object_unref (file_info);
	}

	g_object_unref (attachment);

	g_string_truncate (part_id, part_id_len);

	/* Push to head, not tail. */
	g_queue_push_head (parts_queue, empa);
}