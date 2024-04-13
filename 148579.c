pk_transaction_download_packages (PkTransaction *transaction,
				  GVariant *params,
				  GDBusMethodInvocation *context)
{
	gboolean ret;
	gint retval;
	guint length;
	gboolean store_in_cache;
	g_autoptr(GError) error = NULL;
	g_autofree gchar *directory = NULL;
	g_autofree gchar **package_ids = NULL;
	g_autofree gchar *package_ids_temp = NULL;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	g_variant_get (params, "(b^a&s)",
		       &store_in_cache,
		       &package_ids);

	package_ids_temp = pk_package_ids_to_string (package_ids);
	g_debug ("DownloadPackages method called: %s", package_ids_temp);

	/* not implemented yet */
	if (!pk_backend_is_implemented (transaction->priv->backend,
					PK_ROLE_ENUM_DOWNLOAD_PACKAGES)) {
		g_set_error (&error,
			     PK_TRANSACTION_ERROR,
			     PK_TRANSACTION_ERROR_NOT_SUPPORTED,
			     "DownloadPackages not supported by backend");
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
		goto out;
	}

	/* create cache directory */
	if (!store_in_cache) {
		directory = g_build_filename (LOCALSTATEDIR, "cache", "PackageKit",
					     "downloads", transaction->priv->tid, NULL);
		/* rwxrwxr-x */
		retval = g_mkdir_with_parents (directory, 0775);
		if (retval != 0) {
			g_set_error (&error,
				     PK_TRANSACTION_ERROR,
				     PK_TRANSACTION_ERROR_DENIED,
				     "cannot create %s", directory);
			goto out;
		}
	}

	/* save so we can run later */
	transaction->priv->cached_package_ids = g_strdupv (package_ids);
	transaction->priv->cached_directory = g_strdup (directory);
	pk_transaction_set_role (transaction, PK_ROLE_ENUM_DOWNLOAD_PACKAGES);
	pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_READY);
out:
	pk_transaction_dbus_return (context, error);
}