pk_transaction_offline_invalidate_check (PkTransaction *transaction)
{
	PkPackage *pkg;
	const gchar *package_id;
	gchar **package_ids;
	guint i;
	g_autoptr(GError) error = NULL;
	g_autoptr(PkPackageSack) sack = NULL;
	g_autoptr(GPtrArray) invalidated = NULL;

	/* get the existing prepared updates */
	sack = pk_offline_get_prepared_sack (NULL);
	if (sack == NULL)
		return;

	/* are there any requested packages that match in prepared-updates */
	package_ids = transaction->priv->cached_package_ids;
	for (i = 0; package_ids[i] != NULL; i++) {
		pkg = pk_package_sack_find_by_id_name_arch (sack, package_ids[i]);
		if (pkg != NULL) {
			g_debug ("%s modified %s, invalidating prepared-updates",
				 package_ids[i], pk_package_get_id (pkg));
			if (!pk_offline_auth_invalidate (&error)) {
				g_warning ("failed to invalidate: %s",
					   error->message);
			}
			g_object_unref (pkg);
			return;
		}
	}

	/* are there any changed deps that match a package in prepared-updates */
	invalidated = pk_results_get_package_array (transaction->priv->results);
	for (i = 0; i < invalidated->len; i++) {
		package_id = pk_package_get_id (g_ptr_array_index (invalidated, i));
		pkg = pk_package_sack_find_by_id_name_arch (sack, package_id);
		if (pkg != NULL) {
			g_debug ("%s modified %s, invalidating prepared-updates",
				 package_id, pk_package_get_id (pkg));
			if (!pk_offline_auth_invalidate (&error)) {
				g_warning ("failed to invalidate: %s",
					   error->message);
			}
			g_object_unref (pkg);
			return;
		}
	}
}