pk_transaction_item_progress_cb (PkBackendJob *job,
				 PkItemProgress *item_progress,
				 PkTransaction *transaction)
{
	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	/* emit */
	g_debug ("emitting item-progress %s, %s: %u",
		 pk_item_progress_get_package_id (item_progress),
		 pk_status_enum_to_string (pk_item_progress_get_status (item_progress)),
		 pk_item_progress_get_percentage (item_progress));
	g_dbus_connection_emit_signal (transaction->priv->connection,
				       NULL,
				       transaction->priv->tid,
				       PK_DBUS_INTERFACE_TRANSACTION,
				       "ItemProgress",
				       g_variant_new ("(suu)",
						      pk_item_progress_get_package_id (item_progress),
						      pk_item_progress_get_status (item_progress),
						      pk_item_progress_get_percentage (item_progress)),
				       NULL);
}