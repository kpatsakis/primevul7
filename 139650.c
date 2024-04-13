e_mail_parser_finalize (GObject *object)
{
	EMailParserPrivate *priv;

	priv = E_MAIL_PARSER_GET_PRIVATE (object);

	g_clear_object (&priv->session);
	g_hash_table_destroy (priv->ongoing_part_lists);
	g_mutex_clear (&priv->mutex);

	/* Chain up to parent's finalize() method. */
	G_OBJECT_CLASS (parent_class)->finalize (object);
}