pk_transaction_media_change_required_cb (PkBackend *backend,
					 PkMediaChangeRequired *item,
					 PkTransaction *transaction)
{
	PkMediaTypeEnum media_type;
	g_autofree gchar *media_id = NULL;
	g_autofree gchar *media_text = NULL;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	/* add to results */
	pk_results_add_media_change_required (transaction->priv->results, item);

	/* get data */
	g_object_get (item,
		      "media-type", &media_type,
		      "media-id", &media_id,
		      "media-text", &media_text,
		      NULL);

	/* emit */
	g_debug ("emitting media-change-required %s, %s, %s",
		 pk_media_type_enum_to_string (media_type),
		 media_id,
		 media_text);
	g_dbus_connection_emit_signal (transaction->priv->connection,
				       NULL,
				       transaction->priv->tid,
				       PK_DBUS_INTERFACE_TRANSACTION,
				       "MediaChangeRequired",
				       g_variant_new ("(uss)",
						      media_type,
						      media_id,
						      media_text != NULL ? media_text : ""),
				       NULL);

	/* we should mark this transaction so that we finish with a special code */
	transaction->priv->emit_media_change_required = TRUE;
}