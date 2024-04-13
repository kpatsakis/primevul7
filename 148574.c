pk_transaction_repo_detail_cb (PkBackend *backend,
			       PkRepoDetail *item,
			       PkTransaction *transaction)
{
	gboolean enabled;
	const gchar *repo_id;
	const gchar *description;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	/* add to results */
	pk_results_add_repo_detail (transaction->priv->results, item);

	/* emit */
	repo_id = pk_repo_detail_get_id (item);
	description = pk_repo_detail_get_description (item);
	enabled = pk_repo_detail_get_enabled (item);
	g_debug ("emitting repo-detail %s, %s, %i", repo_id, description, enabled);
	g_dbus_connection_emit_signal (transaction->priv->connection,
				       NULL,
				       transaction->priv->tid,
				       PK_DBUS_INTERFACE_TRANSACTION,
				       "RepoDetail",
				       g_variant_new ("(ssb)",
						      repo_id,
						      description != NULL ? description : "",
						      enabled),
				       NULL);
}