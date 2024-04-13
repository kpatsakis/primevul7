e_mail_parser_init (EMailParser *parser)
{
	parser->priv = E_MAIL_PARSER_GET_PRIVATE (parser);
	parser->priv->ongoing_part_lists = g_hash_table_new (g_direct_hash, g_direct_equal);

	g_mutex_init (&parser->priv->mutex);
}