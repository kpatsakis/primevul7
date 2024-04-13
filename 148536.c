pk_transaction_upgrade_system (PkTransaction *transaction,
			       GVariant *params,
			       GDBusMethodInvocation *context)
{
	gboolean ret;
	PkBitfield transaction_flags;
	PkUpgradeKindEnum upgrade_kind;
	const gchar *distro_id;
	g_autoptr(GError) error = NULL;
	g_autofree gchar *transaction_flags_temp = NULL;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	g_variant_get (params, "(t&su)",
		       &transaction_flags,
		       &distro_id,
		       &upgrade_kind);

	transaction_flags_temp = pk_transaction_flag_bitfield_to_string (transaction_flags);
	g_debug ("UpgradeSystem method called: %s: %s  (transaction_flags: %s)",
		 distro_id,
		 pk_upgrade_kind_enum_to_string (upgrade_kind),
		 transaction_flags_temp);

	/* not implemented yet */
	if (!pk_backend_is_implemented (transaction->priv->backend,
					PK_ROLE_ENUM_UPGRADE_SYSTEM)) {
		g_set_error (&error,
			     PK_TRANSACTION_ERROR,
			     PK_TRANSACTION_ERROR_NOT_SUPPORTED,
			     "UpgradeSystem not supported by backend");
		pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
		goto out;
	}

	/* save so we can run later */
	transaction->priv->cached_transaction_flags = transaction_flags;
	transaction->priv->cached_value = g_strdup (distro_id);
	transaction->priv->cached_upgrade_kind = upgrade_kind;
	pk_transaction_set_role (transaction, PK_ROLE_ENUM_UPGRADE_SYSTEM);

	/* this changed */
	pk_transaction_emit_property_changed (transaction,
					      "TransactionFlags",
					      g_variant_new_uint64 (transaction_flags));

	/* try to get authorization */
	ret = pk_transaction_obtain_authorization (transaction,
						   PK_ROLE_ENUM_UPGRADE_SYSTEM,
						   &error);
	if (!ret) {
		pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
		goto out;
	}
out:
	pk_transaction_dbus_return (context, error);
}