pk_transaction_get_files_local (PkTransaction *transaction,
				  GVariant *params,
				  GDBusMethodInvocation *context)
{
	gboolean ret;
	GError *error_local = NULL;
	guint i;
	guint length;
	g_autoptr(GError) error = NULL;
	g_autofree gchar *content_type = NULL;
	g_autofree gchar *files_temp = NULL;
	g_autofree gchar **full_paths = NULL;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	g_variant_get (params, "(^a&s)", &full_paths);

	files_temp = pk_package_ids_to_string (full_paths);
	g_debug ("GetFilesLocal method called: %s", files_temp);

	/* not implemented yet */
	if (!pk_backend_is_implemented (transaction->priv->backend,
					PK_ROLE_ENUM_GET_FILES_LOCAL)) {
		g_set_error (&error,
			     PK_TRANSACTION_ERROR,
			     PK_TRANSACTION_ERROR_NOT_SUPPORTED,
			     "GetFilesLocal not supported by backend");
		pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
		goto out;
	}

	/* check for length sanity */
	length = g_strv_length (full_paths);
	if (length == 0) {
		g_set_error_literal (&error,
				     PK_TRANSACTION_ERROR,
				     PK_TRANSACTION_ERROR_NUMBER_OF_PACKAGES_INVALID,
				     "No filenames listed");
		pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
		goto out;
	}
	if (length > PK_TRANSACTION_MAX_PACKAGES_TO_PROCESS) {
		g_set_error (&error,
			     PK_TRANSACTION_ERROR,
			     PK_TRANSACTION_ERROR_NUMBER_OF_PACKAGES_INVALID,
			     "Too many files to process (%i/%i)", length, PK_TRANSACTION_MAX_PACKAGES_TO_PROCESS);
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
		content_type = pk_transaction_get_content_type_for_file (full_paths[i],
								         &error_local);
		if (content_type == NULL) {
			g_set_error (&error,
				     PK_TRANSACTION_ERROR,
				     PK_TRANSACTION_ERROR_MIME_TYPE_NOT_SUPPORTED,
				     "Failed to get content type for file %s",
				     full_paths[i]);
			pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
			goto out;
		}

		/* supported content type? */
		ret = pk_transaction_is_supported_content_type (transaction, content_type);
		if (!ret) {
			g_set_error (&error,
				     PK_TRANSACTION_ERROR,
				     PK_TRANSACTION_ERROR_MIME_TYPE_NOT_SUPPORTED,
				     "MIME type '%s' not supported %s",
				     content_type, full_paths[i]);
			pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
			goto out;
		}
	}

	/* save so we can run later */
	transaction->priv->cached_full_paths = g_strdupv (full_paths);
	pk_transaction_set_role (transaction, PK_ROLE_ENUM_GET_FILES_LOCAL);
	pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_READY);
out:
	pk_transaction_dbus_return (context, error);
}