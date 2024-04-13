e_mail_parser_new (CamelSession *session)
{
	g_return_val_if_fail (CAMEL_IS_SESSION (session), NULL);

	return g_object_new (
		E_TYPE_MAIL_PARSER,
		"session", session, NULL);
}