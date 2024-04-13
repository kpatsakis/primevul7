pk_transaction_finalize (GObject *object)
{
	PkTransaction *transaction;

	g_return_if_fail (PK_IS_TRANSACTION (object));

	transaction = PK_TRANSACTION (object);

	if (transaction->priv->subject != NULL)
		g_object_unref (transaction->priv->subject);
	if (transaction->priv->watch_id > 0)
		g_bus_unwatch_name (transaction->priv->watch_id);
	g_free (transaction->priv->last_package_id);
	g_free (transaction->priv->cached_package_id);
	g_free (transaction->priv->cached_key_id);
	g_strfreev (transaction->priv->cached_package_ids);
	g_free (transaction->priv->cached_transaction_id);
	g_free (transaction->priv->cached_directory);
	g_strfreev (transaction->priv->cached_values);
	g_free (transaction->priv->cached_repo_id);
	g_free (transaction->priv->cached_parameter);
	g_free (transaction->priv->cached_value);
	g_free (transaction->priv->tid);
	g_free (transaction->priv->sender);
	g_free (transaction->priv->cmdline);
	g_ptr_array_unref (transaction->priv->supported_content_types);

	if (transaction->priv->connection != NULL)
		g_object_unref (transaction->priv->connection);
	if (transaction->priv->introspection != NULL)
		g_dbus_node_info_unref (transaction->priv->introspection);

	g_key_file_unref (transaction->priv->conf);
	g_object_unref (transaction->priv->dbus);
	if (transaction->priv->backend != NULL)
		g_object_unref (transaction->priv->backend);
	g_object_unref (transaction->priv->job);
	g_object_unref (transaction->priv->transaction_db);
	g_object_unref (transaction->priv->results);
	g_object_unref (transaction->priv->authority);
	g_object_unref (transaction->priv->cancellable);

	G_OBJECT_CLASS (pk_transaction_parent_class)->finalize (object);
}