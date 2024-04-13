pk_transaction_set_backend (PkTransaction *transaction,
			    PkBackend *backend)
{
	/* save a reference */
	if (transaction->priv->backend != NULL)
		g_object_unref (transaction->priv->backend);
	transaction->priv->backend = g_object_ref (backend);

	/* setup supported mime types */
	pk_transaction_setup_mime_types (transaction);
}