pk_transaction_get_distro_upgrades (PkTransaction *transaction,
				    GVariant *params,
				    GDBusMethodInvocation *context)
{
	g_autoptr(GError) error = NULL;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	g_debug ("GetDistroUpgrades method called");

	/* not implemented yet */
	if (!pk_backend_is_implemented (transaction->priv->backend,
					PK_ROLE_ENUM_GET_DISTRO_UPGRADES)) {
		g_set_error (&error,
			     PK_TRANSACTION_ERROR,
			     PK_TRANSACTION_ERROR_NOT_SUPPORTED,
			     "GetDistroUpgrades not supported by backend");
		pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
		goto out;
	}

	/* save so we can run later */
	pk_transaction_set_role (transaction, PK_ROLE_ENUM_GET_DISTRO_UPGRADES);
	pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_READY);
out:
	pk_transaction_dbus_return (context, error);
}