e_mail_parser_parse_part_as (EMailParser *parser,
                             CamelMimePart *part,
                             GString *part_id,
                             const gchar *mime_type,
                             GCancellable *cancellable,
                             GQueue *out_mail_parts)
{
	GQueue *parsers;
	GList *iter;
	gboolean mime_part_handled = FALSE;

	parsers = e_mail_parser_get_parsers (parser, mime_type);

	if (parsers == NULL) {
		e_mail_parser_wrap_as_attachment (
			parser, part, part_id, out_mail_parts);
		return TRUE;
	}

	for (iter = parsers->head; iter; iter = iter->next) {
		EMailParserExtension *extension;

		extension = iter->data;
		if (!extension)
			continue;

		mime_part_handled = e_mail_parser_extension_parse (
			extension, parser, part, part_id,
			cancellable, out_mail_parts);

		if (mime_part_handled)
			break;
	}

	return mime_part_handled;
}