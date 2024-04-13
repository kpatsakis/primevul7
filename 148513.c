pk_transaction_offline_finished (PkTransaction *transaction)
{
	PkBitfield transaction_flags;
	gchar **package_ids;
	g_autoptr(GError) error = NULL;
	g_autoptr(GPtrArray) array = NULL;

	/* if we're doing UpdatePackages[only-download] then update the
	 * prepared-updates file */
	transaction_flags = transaction->priv->cached_transaction_flags;
	if (transaction->priv->role == PK_ROLE_ENUM_UPDATE_PACKAGES &&
	    pk_bitfield_contain (transaction_flags,
				 PK_TRANSACTION_FLAG_ENUM_ONLY_DOWNLOAD)) {
		package_ids = transaction->priv->cached_package_ids;
		if (!pk_offline_auth_set_prepared_ids (package_ids, &error)) {
			g_warning ("failed to write offline update: %s",
				   error->message);
		}
		return;
	}

	/* if we're doing UpgradeSystem[only-download] then update the
	 * prepared-upgrade file */
	transaction_flags = transaction->priv->cached_transaction_flags;
	if (transaction->priv->role == PK_ROLE_ENUM_UPGRADE_SYSTEM &&
	    pk_bitfield_contain (transaction_flags,
				 PK_TRANSACTION_FLAG_ENUM_ONLY_DOWNLOAD)) {
		const gchar *version = transaction->priv->cached_value;
		g_autofree gchar *name = NULL;

		name = pk_get_distro_name (&error);
		if (name == NULL) {
			g_warning ("failed to get distro name: %s",
				   error->message);
			return;
		}
		if (!pk_offline_auth_set_prepared_upgrade (name, version, &error)) {
			g_warning ("failed to write offline system upgrade state: %s",
				   error->message);
			return;
		}
		return;
	}

	switch (transaction->priv->role) {
	case PK_ROLE_ENUM_GET_UPDATES:
		/* if we do get-updates and there's no updates then remove
		 * prepared-updates so the UI doesn't display update & reboot */
		array = pk_results_get_package_array (transaction->priv->results);
		if (array->len == 0) {
			if (!pk_offline_auth_invalidate (&error)) {
				g_warning ("failed to invalidate: %s",
					   error->message);
			}
		}
		break;
	case PK_ROLE_ENUM_REFRESH_CACHE:
	case PK_ROLE_ENUM_REPO_SET_DATA:
	case PK_ROLE_ENUM_REPO_ENABLE:
		/* delete the prepared updates file as it's not valid */
		if (!pk_offline_auth_invalidate (&error))
			g_warning ("failed to invalidate: %s", error->message);
		break;
	case PK_ROLE_ENUM_UPDATE_PACKAGES:
	case PK_ROLE_ENUM_INSTALL_PACKAGES:
	case PK_ROLE_ENUM_REMOVE_PACKAGES:
		/* delete the file if the action affected any package
		 * already listed in the prepared updates file */
		pk_transaction_offline_invalidate_check (transaction);
		break;
	default:
		break;
	}
}