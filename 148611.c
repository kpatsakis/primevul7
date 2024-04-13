pk_transaction_new (GKeyFile *conf, GDBusNodeInfo *introspection)
{
	PkTransaction *transaction;
	transaction = g_object_new (PK_TYPE_TRANSACTION, NULL);
	transaction->priv->conf = g_key_file_ref (conf);
	transaction->priv->job = pk_backend_job_new (conf);
	transaction->priv->introspection = g_dbus_node_info_ref (introspection);
	return PK_TRANSACTION (transaction);
}