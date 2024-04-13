pk_transaction_set_tid (PkTransaction *transaction, const gchar *tid)
{
	static const GDBusInterfaceVTable interface_vtable = {
		pk_transaction_method_call,
		pk_transaction_get_property,
		NULL
	};

	g_return_val_if_fail (PK_IS_TRANSACTION (transaction), FALSE);
	g_return_val_if_fail (tid != NULL, FALSE);
	g_return_val_if_fail (transaction->priv->tid == NULL, FALSE);

	transaction->priv->tid = g_strdup (tid);

	/* register org.freedesktop.PackageKit.Transaction */
	transaction->priv->connection = g_bus_get_sync (G_BUS_TYPE_SYSTEM, NULL, NULL);
	g_assert (transaction->priv->connection != NULL);
	transaction->priv->registration_id =
		g_dbus_connection_register_object (transaction->priv->connection,
						   tid,
						   transaction->priv->introspection->interfaces[0],
						   &interface_vtable,
						   transaction,  /* user_data */
						   NULL,  /* user_data_free_func */
						   NULL); /* GError** */
	g_assert (transaction->priv->registration_id > 0);
	return TRUE;
}