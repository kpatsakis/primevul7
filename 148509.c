pk_transaction_install_files (PkTransaction *transaction,
			      GVariant *params,
			      GDBusMethodInvocation *context)
{
	gboolean ret;
	GError *error_local = NULL;
	guint length;
	guint i;
	PkBitfield transaction_flags;
	g_autoptr(GError) error = NULL;
	g_autofree gchar *content_type = NULL;
	g_autofree gchar **full_paths = NULL;
	g_autofree gchar *full_paths_temp = NULL;
	g_autofree gchar *transaction_flags_temp = NULL;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	g_variant_get (params, "(t^a&s)",
		       &transaction_flags,
		       &full_paths);

	full_paths_temp = pk_package_ids_to_string (full_paths);
	transaction_flags_temp = pk_transaction_flag_bitfield_to_string (transaction_flags);
	g_debug ("InstallFiles method called: %s (transaction_flags: %s)",
		 full_paths_temp, transaction_flags_temp);

	/* not implemented yet */
	if (!pk_backend_is_implemented (transaction->priv->backend,
					PK_ROLE_ENUM_INSTALL_FILES)) {
		g_set_error (&error,
			     PK_TRANSACTION_ERROR,
			     PK_TRANSACTION_ERROR_NOT_SUPPORTED,
			     "InstallFiles not supported by backend");
		pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
		goto out;
	}

	/* check all files exists and are valid */
	length = g_strv_length (full_paths);
	for (i = 0; i < length; i++) {
		/* exists */
		ret = g_file_test (full_paths[i], G_FILE_TEST_EXISTS);
		if (!ret) {
			g_set_error (&error,
				     PK_TRANSACTION_ERROR,
				     PK_TRANSACTION_ERROR_NO_SUCH_FILE,
				     "No such file %s", full_paths[i]);
			pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
				goto out;
		}

		/* get content type */
		content_type = pk_transaction_get_content_type_for_file (full_paths[i], &error_local);
		if (content_type == NULL) {
			g_set_error (&error,
				     PK_TRANSACTION_ERROR,
				     PK_TRANSACTION_ERROR_NOT_SUPPORTED,
				     "Failed to get content type for file %s",
				     full_paths[i]);
			pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
				goto out;
		}

		/* supported content type? */
		ret = pk_transaction_is_supported_content_type (transaction, content_type);
		if (!ret) {
			if (g_strcmp0 ("application/x-app-package", content_type) == 0 ||
			    g_str_has_suffix (full_paths[i], ".ipk") == TRUE) {
				g_set_error (&error,
					      PK_TRANSACTION_ERROR,
					      PK_TRANSACTION_ERROR_MIME_TYPE_NOT_SUPPORTED,
					      "Listaller is required to install %s", full_paths[i]);
			} else {
				g_set_error (&error,
					     PK_TRANSACTION_ERROR,
					     PK_TRANSACTION_ERROR_MIME_TYPE_NOT_SUPPORTED,
					     "MIME type '%s' not supported %s",
					     content_type, full_paths[i]);
			}
			pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
				goto out;
		}
	}

	/* save so we can run later */
	transaction->priv->cached_transaction_flags = transaction_flags;
	transaction->priv->cached_full_paths = g_strdupv (full_paths);
	pk_transaction_set_role (transaction, PK_ROLE_ENUM_INSTALL_FILES);

	/* this changed */
	pk_transaction_emit_property_changed (transaction,
					      "TransactionFlags",
					      g_variant_new_uint64 (transaction_flags));

	/* try to get authorization */
	ret = pk_transaction_obtain_authorization (transaction,
						   PK_ROLE_ENUM_INSTALL_FILES,
						   &error);
	if (!ret) {
		pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
		goto out;
	}
out:
	pk_transaction_dbus_return (context, error);
}