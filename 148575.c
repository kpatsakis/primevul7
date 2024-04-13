pk_transaction_package_cb (PkBackend *backend,
			   PkPackage *item,
			   PkTransaction *transaction)
{
	const gchar *role_text;
	PkInfoEnum info;
	const gchar *package_id;
	const gchar *summary = NULL;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	/* have we already been marked as finished? */
	if (transaction->priv->finished) {
		g_warning ("Already finished");
		return;
	}

	/* check the backend is doing the right thing */
	info = pk_package_get_info (item);
	if (transaction->priv->role == PK_ROLE_ENUM_INSTALL_PACKAGES ||
	    transaction->priv->role == PK_ROLE_ENUM_UPDATE_PACKAGES) {
		if (info == PK_INFO_ENUM_INSTALLED) {
			role_text = pk_role_enum_to_string (transaction->priv->role);
			g_warning ("%s emitted 'installed' rather than 'installing'",
				   role_text);
			return;
		}
	}

	/* check we are respecting the filters */
	if (pk_bitfield_contain (transaction->priv->cached_filters,
				 PK_FILTER_ENUM_NOT_INSTALLED)) {
		if (info == PK_INFO_ENUM_INSTALLED) {
			role_text = pk_role_enum_to_string (transaction->priv->role);
			g_warning ("%s emitted package that was installed when "
				   "the ~installed filter is in place",
				   role_text);
			return;
		}
	}
	if (pk_bitfield_contain (transaction->priv->cached_filters,
				 PK_FILTER_ENUM_INSTALLED)) {
		if (info == PK_INFO_ENUM_AVAILABLE) {
			role_text = pk_role_enum_to_string (transaction->priv->role);
			g_warning ("%s emitted package that was ~installed when "
				   "the installed filter is in place",
				   role_text);
			return;
		}
	}

	/* add to results even if we already got a result */
	if (info != PK_INFO_ENUM_FINISHED)
		pk_results_add_package (transaction->priv->results, item);

	/* emit */
	package_id = pk_package_get_id (item);
	g_free (transaction->priv->last_package_id);
	transaction->priv->last_package_id = g_strdup (package_id);
	summary = pk_package_get_summary (item);
	if (transaction->priv->role != PK_ROLE_ENUM_GET_PACKAGES) {
		g_debug ("emit package %s, %s, %s",
			 pk_info_enum_to_string (info),
			 package_id,
			 summary);
	}
	g_dbus_connection_emit_signal (transaction->priv->connection,
				       NULL,
				       transaction->priv->tid,
				       PK_DBUS_INTERFACE_TRANSACTION,
				       "Package",
				       g_variant_new ("(uss)",
						      info,
						      package_id,
						      summary ? summary : ""),
				       NULL);
}