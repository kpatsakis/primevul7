e_mail_parser_get_extension_registry (EMailParser *parser)
{
	EMailParserClass *parser_class;

	g_return_val_if_fail (E_IS_MAIL_PARSER (parser), NULL);

	parser_class = E_MAIL_PARSER_GET_CLASS (parser);
	g_return_val_if_fail (parser_class != NULL, NULL);

	return E_MAIL_EXTENSION_REGISTRY (parser_class->extension_registry);
}