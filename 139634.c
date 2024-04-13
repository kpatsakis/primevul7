e_mail_parser_parse_part (EMailParser *parser,
                          CamelMimePart *part,
                          GString *part_id,
                          GCancellable *cancellable,
                          GQueue *out_mail_parts)
{
	CamelContentType *ct;
	gchar *mime_type;
	gint handled;

	ct = camel_mime_part_get_content_type (part);
	if (!ct) {
		mime_type = (gchar *) "application/vnd.evolution.error";
	} else {
		gchar *tmp;
		tmp = camel_content_type_simple (ct);
		mime_type = g_ascii_strdown (tmp, -1);
		g_free (tmp);
	}

	handled = e_mail_parser_parse_part_as (
		parser, part, part_id, mime_type,
		cancellable, out_mail_parts);

	if (ct) {
		g_free (mime_type);
	}

	return handled;
}