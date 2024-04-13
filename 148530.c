pk_transaction_repo_signature_required_cb (PkBackend *backend,
					   PkRepoSignatureRequired *item,
					   PkTransaction *transaction)
{
	PkSigTypeEnum type;
	g_autofree gchar *package_id = NULL;
	g_autofree gchar *repository_name = NULL;
	g_autofree gchar *key_url = NULL;
	g_autofree gchar *key_userid = NULL;
	g_autofree gchar *key_id = NULL;
	g_autofree gchar *key_fingerprint = NULL;
	g_autofree gchar *key_timestamp = NULL;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	/* add to results */
	pk_results_add_repo_signature_required (transaction->priv->results, item);

	/* get data */
	g_object_get (item,
		      "type", &type,
		      "package-id", &package_id,
		      "repository-name", &repository_name,
		      "key-url", &key_url,
		      "key-userid", &key_userid,
		      "key-id", &key_id,
		      "key-fingerprint", &key_fingerprint,
		      "key-timestamp", &key_timestamp,
		      NULL);

	/* emit */
	g_debug ("emitting repo_signature_required %s, %s, %s, %s, %s, %s, %s, %s",
		 package_id, repository_name, key_url, key_userid, key_id,
		 key_fingerprint, key_timestamp,
		 pk_sig_type_enum_to_string (type));
	g_dbus_connection_emit_signal (transaction->priv->connection,
				       NULL,
				       transaction->priv->tid,
				       PK_DBUS_INTERFACE_TRANSACTION,
				       "RepoSignatureRequired",
				       g_variant_new ("(sssssssu)",
						      package_id,
						      repository_name,
						      key_url != NULL ? key_url : "",
						      key_userid != NULL ? key_userid : "",
						      key_id != NULL ? key_id : "",
						      key_fingerprint != NULL ? key_fingerprint : "",
						      key_timestamp != NULL ? key_timestamp : "",
						      type),
				       NULL);

	/* we should mark this transaction so that we finish with a special code */
	transaction->priv->emit_signature_required = TRUE;
}