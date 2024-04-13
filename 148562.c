pk_transaction_require_restart_cb (PkBackend *backend,
				   PkRequireRestart *item,
				   PkTransaction *transaction)
{
	PkRequireRestart *item_tmp;
	gboolean found = FALSE;
	guint i;
	PkRestartEnum restart;
	g_autofree gchar *package_id = NULL;
	g_autoptr(GPtrArray) array = NULL;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	/* get data */
	g_object_get (item,
		      "package-id", &package_id,
		      "restart", &restart,
		      NULL);

	/* filter out duplicates */
	array = pk_results_get_require_restart_array (transaction->priv->results);
	for (i = 0; i < array->len; i++) {
		g_autofree gchar *package_id_tmp = NULL;
		item_tmp = g_ptr_array_index (array, i);
		g_object_get (item_tmp,
			      "package-id", &package_id_tmp,
			      NULL);
		if (g_strcmp0 (package_id, package_id_tmp) == 0) {
			found = TRUE;
			break;
		}
	}

	/* ignore */
	if (found) {
		g_debug ("ignoring %s (%s) as already sent",
			 pk_restart_enum_to_string (restart),
			 package_id);
		return;
	}

	/* add to results */
	pk_results_add_require_restart (transaction->priv->results, item);

	/* emit */
	g_debug ("emitting require-restart %s, '%s'",
		 pk_restart_enum_to_string (restart),
		 package_id);
	g_dbus_connection_emit_signal (transaction->priv->connection,
				       NULL,
				       transaction->priv->tid,
				       PK_DBUS_INTERFACE_TRANSACTION,
				       "RequireRestart",
				       g_variant_new ("(us)",
						      restart,
						      package_id),
				       NULL);
}