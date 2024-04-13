pk_transaction_accept_eula (PkTransaction *transaction,
			    GVariant *params,
			    GDBusMethodInvocation *context)
{
	gboolean ret;
	guint idle_id;
	const gchar *eula_id = NULL;
	g_autoptr(GError) error = NULL;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	g_variant_get (params, "(&s)",
		       &eula_id);

	/* check for sanity */
	ret = pk_transaction_strvalidate (eula_id, &error);
	if (!ret) {
		pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
		goto out;
	}

	pk_transaction_set_role (transaction, PK_ROLE_ENUM_ACCEPT_EULA);

	/* try to get authorization */
	ret = pk_transaction_obtain_authorization (transaction,
						   PK_ROLE_ENUM_ACCEPT_EULA,
						   &error);
	if (!ret) {
		pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
		goto out;
	}

	g_debug ("AcceptEula method called: %s", eula_id);
	pk_backend_accept_eula (transaction->priv->backend, eula_id);

	/* we are done */
	idle_id = g_idle_add ((GSourceFunc) pk_transaction_finished_idle_cb, transaction);
	g_source_set_name_by_id (idle_id, "[PkTransaction] finished from accept");
out:
	pk_transaction_dbus_return (context, error);
}