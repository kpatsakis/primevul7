pk_transaction_init (PkTransaction *transaction)
{
	gboolean ret;
	g_autoptr(GError) error = NULL;
	transaction->priv = PK_TRANSACTION_GET_PRIVATE (transaction);
	transaction->priv->allow_cancel = TRUE;
	transaction->priv->caller_active = TRUE;
	transaction->priv->cached_transaction_flags = PK_TRANSACTION_FLAG_ENUM_NONE;
	transaction->priv->cached_filters = PK_FILTER_ENUM_NONE;
	transaction->priv->uid = PK_TRANSACTION_UID_INVALID;
	transaction->priv->role = PK_ROLE_ENUM_UNKNOWN;
	transaction->priv->status = PK_STATUS_ENUM_WAIT;
	transaction->priv->percentage = PK_BACKEND_PERCENTAGE_INVALID;
	transaction->priv->state = PK_TRANSACTION_STATE_UNKNOWN;
	transaction->priv->dbus = pk_dbus_new ();
	transaction->priv->results = pk_results_new ();
	transaction->priv->supported_content_types = g_ptr_array_new_with_free_func (g_free);
	transaction->priv->authority = polkit_authority_get_sync (NULL, &error);
	if (transaction->priv->authority == NULL)
		g_error ("failed to get pokit authority: %s", error->message);
	transaction->priv->cancellable = g_cancellable_new ();

	transaction->priv->transaction_db = pk_transaction_db_new ();
	ret = pk_transaction_db_load (transaction->priv->transaction_db, &error);
	if (!ret)
		g_error ("PkEngine: failed to load transaction db: %s", error->message);
}