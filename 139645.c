e_mail_parser_get_session (EMailParser *parser)
{
	g_return_val_if_fail (E_IS_MAIL_PARSER (parser), NULL);

	return parser->priv->session;
}