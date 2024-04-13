pk_transaction_remove_packages (PkTransaction *transaction,
				GVariant *params,
				GDBusMethodInvocation *context)
{
	gboolean ret;
	guint length;
	gboolean allow_deps;
	gboolean autoremove;
	PkBitfield transaction_flags;
	g_autoptr(GError) error = NULL;
	g_autofree gchar **package_ids = NULL;
	g_autofree gchar *package_ids_temp = NULL;
	g_autofree gchar *transaction_flags_temp = NULL;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	g_variant_get (params, "(t^a&sbb)",
		       &transaction_flags,
		       &package_ids,
		       &allow_deps,
		       &autoremove);

	package_ids_temp = pk_package_ids_to_string (package_ids);
	transaction_flags_temp = pk_transaction_flag_bitfield_to_string (transaction_flags);
	g_debug ("RemovePackages method called: %s, %i, %i (transaction_flags: %s)",
		 package_ids_temp, allow_deps, autoremove, transaction_flags_temp);

	/* not implemented yet */
	if (!pk_backend_is_implemented (transaction->priv->backend,
					PK_ROLE_ENUM_REMOVE_PACKAGES)) {
		g_set_error (&error,
			     PK_TRANSACTION_ERROR,
			     PK_TRANSACTION_ERROR_NOT_SUPPORTED,
			     "RemovePackages not supported by backend");
		pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
		goto out;
	}

	/* check for length sanity */
	length = g_strv_length (package_ids);
	if (length > PK_TRANSACTION_MAX_PACKAGES_TO_PROCESS) {
		g_set_error (&error,
			     PK_TRANSACTION_ERROR,
			     PK_TRANSACTION_ERROR_NUMBER_OF_PACKAGES_INVALID,
			     "Too many packages to process (%i/%i)",
			     length, PK_TRANSACTION_MAX_PACKAGES_TO_PROCESS);
		pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
		goto out;
	}

	/* check package_ids */
	ret = pk_package_ids_check (package_ids);
	if (!ret) {
		g_set_error (&error,
			     PK_TRANSACTION_ERROR,
			     PK_TRANSACTION_ERROR_PACKAGE_ID_INVALID,
			     "The package id's '%s' are not valid", package_ids_temp);
		pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
		goto out;
	}

	/* save so we can run later */
	transaction->priv->cached_transaction_flags = transaction_flags;
	transaction->priv->cached_package_ids = g_strdupv (package_ids);
	transaction->priv->cached_allow_deps = allow_deps;
	transaction->priv->cached_autoremove = autoremove;
	pk_transaction_set_role (transaction, PK_ROLE_ENUM_REMOVE_PACKAGES);

	/* this changed */
	pk_transaction_emit_property_changed (transaction,
					      "TransactionFlags",
					      g_variant_new_uint64 (transaction_flags));

	/* try to get authorization */
	ret = pk_transaction_obtain_authorization (transaction,
						   PK_ROLE_ENUM_REMOVE_PACKAGES,
						   &error);
	if (!ret) {
		pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
		goto out;
	}
out:
	pk_transaction_dbus_return (context, error);
}