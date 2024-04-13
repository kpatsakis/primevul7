pk_transaction_get_property (GDBusConnection *connection_, const gchar *sender,
			     const gchar *object_path, const gchar *interface_name,
			     const gchar *property_name, GError **error,
			     gpointer user_data)
{
	PkTransaction *transaction = PK_TRANSACTION (user_data);
	PkTransactionPrivate *priv = transaction->priv;

	if (g_strcmp0 (property_name, "Role") == 0)
		return g_variant_new_uint32 (priv->role);
	if (g_strcmp0 (property_name, "Status") == 0)
		return g_variant_new_uint32 (priv->status);
	if (g_strcmp0 (property_name, "LastPackage") == 0)
		return _g_variant_new_maybe_string (priv->last_package_id);
	if (g_strcmp0 (property_name, "Uid") == 0)
		return g_variant_new_uint32 (priv->uid);
	if (g_strcmp0 (property_name, "Percentage") == 0)
		return g_variant_new_uint32 (transaction->priv->percentage);
	if (g_strcmp0 (property_name, "AllowCancel") == 0)
		return g_variant_new_boolean (priv->allow_cancel);
	if (g_strcmp0 (property_name, "CallerActive") == 0)
		return g_variant_new_boolean (priv->caller_active);
	if (g_strcmp0 (property_name, "ElapsedTime") == 0)
		return g_variant_new_uint32 (priv->elapsed_time);
	if (g_strcmp0 (property_name, "Speed") == 0)
		return g_variant_new_uint32 (priv->speed);
	if (g_strcmp0 (property_name, "DownloadSizeRemaining") == 0)
		return g_variant_new_uint64 (priv->download_size_remaining);
	if (g_strcmp0 (property_name, "TransactionFlags") == 0)
		return g_variant_new_uint64 (priv->cached_transaction_flags);
	return NULL;
}