pk_transaction_distro_upgrade_cb (PkBackendJob *job,
				  PkDistroUpgrade *item,
				  PkTransaction *transaction)
{
	PkUpdateStateEnum state;
	g_autofree gchar *name = NULL;
	g_autofree gchar *summary = NULL;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	/* add to results */
	pk_results_add_distro_upgrade (transaction->priv->results, item);

	/* get data */
	g_object_get (item,
		      "state", &state,
		      "name", &name,
		      "summary", &summary,
		      NULL);

	/* emit */
	g_debug ("emitting distro-upgrade %s, %s, %s",
		 pk_distro_upgrade_enum_to_string (state),
		 name, summary);
	g_dbus_connection_emit_signal (transaction->priv->connection,
				       NULL,
				       transaction->priv->tid,
				       PK_DBUS_INTERFACE_TRANSACTION,
				       "DistroUpgrade",
				       g_variant_new ("(uss)",
						      state,
						      name,
						      summary != NULL ? summary : ""),
				       NULL);
}