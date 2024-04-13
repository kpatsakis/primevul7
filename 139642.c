e_mail_parser_get_parsers (EMailParser *parser,
			   const gchar *mime_type)
{
	EMailExtensionRegistry *reg;
	EMailParserClass *parser_class;
	gchar *as_mime_type;
	GQueue *parsers;

	g_return_val_if_fail (E_IS_MAIL_PARSER (parser), NULL);

	parser_class = E_MAIL_PARSER_GET_CLASS (parser);
	g_return_val_if_fail (parser_class != NULL, NULL);

	if (mime_type)
		as_mime_type = g_ascii_strdown (mime_type, -1);
	else
		as_mime_type = NULL;

	reg = E_MAIL_EXTENSION_REGISTRY (parser_class->extension_registry);

	parsers = e_mail_extension_registry_get_for_mime_type (reg, as_mime_type);
	if (!parsers)
		parsers = e_mail_extension_registry_get_fallback (reg, as_mime_type);

	g_free (as_mime_type);

	return parsers;
}