e_mail_parser_ref_part_list_for_operation (EMailParser *parser,
					   GCancellable *operation)
{
	EMailPartList *part_list;

	g_return_val_if_fail (E_IS_MAIL_PARSER (parser), NULL);

	g_mutex_lock (&parser->priv->mutex);
	part_list = g_hash_table_lookup (parser->priv->ongoing_part_lists, operation);
	if (part_list)
		g_object_ref (part_list);
	g_mutex_unlock (&parser->priv->mutex);

	return part_list;
}