e_mail_parser_get_parsers_for_part (EMailParser *parser,
				    CamelMimePart *part)
{
	CamelContentType *ct;
	gchar *mime_type;
	GQueue *parsers;

	g_return_val_if_fail (E_IS_MAIL_PARSER (parser), NULL);
	g_return_val_if_fail (CAMEL_IS_MIME_PART (part), NULL);

	ct = camel_mime_part_get_content_type (part);
	if (!ct) {
		mime_type = (gchar *) "application/vnd.evolution.error";
	} else {
		gchar *tmp;
		tmp = camel_content_type_simple (ct);
		mime_type = g_ascii_strdown (tmp, -1);
		g_free (tmp);
	}

	parsers = e_mail_parser_get_parsers (parser, mime_type);

	if (ct)
		g_free (mime_type);

	return parsers;
}