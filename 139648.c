mail_parser_set_session (EMailParser *parser,
                         CamelSession *session)
{
	g_return_if_fail (CAMEL_IS_SESSION (session));
	g_return_if_fail (parser->priv->session == NULL);

	parser->priv->session = g_object_ref (session);
}