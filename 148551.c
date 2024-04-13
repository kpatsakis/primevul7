pk_transaction_repo_remove (PkTransaction *transaction,
			    GVariant *params,
			    GDBusMethodInvocation *context)
{
	PkBitfield transaction_flags;
	const gchar *repo_id;
	gboolean autoremove;
	gboolean ret;
	g_autoptr(GError) error = NULL;
	g_autofree gchar *tmp = NULL;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	g_variant_get (params, "(t&sb)",
		       &transaction_flags,
		       &repo_id,
		       &autoremove);

	tmp = pk_transaction_flag_bitfield_to_string (transaction_flags);
	g_debug ("RepoRemove method called: %s, %s, %i",
		 tmp, repo_id, autoremove);

	/* not implemented yet */
	if (!pk_backend_is_implemented (transaction->priv->backend,
					PK_ROLE_ENUM_REPO_REMOVE)) {
		g_set_error_literal (&error,
				     PK_TRANSACTION_ERROR,
				     PK_TRANSACTION_ERROR_NOT_SUPPORTED,
				     "RepoSetData not supported by backend");
		pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
		goto out;
	}

	/* check for sanity */
	ret = pk_transaction_strvalidate (repo_id, &error);
	if (!ret) {
		pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
		goto out;
	}

	/* save so we can run later */
	transaction->priv->cached_repo_id = g_strdup (repo_id);
	transaction->priv->cached_transaction_flags = transaction_flags;
	transaction->priv->cached_autoremove = autoremove;
	pk_transaction_set_role (transaction, PK_ROLE_ENUM_REPO_REMOVE);

	/* try to get authorization */
	ret = pk_transaction_obtain_authorization (transaction,
						   PK_ROLE_ENUM_REPO_REMOVE,
						   &error);
	if (!ret) {
		pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_ERROR);
		goto out;
	}
out:
	pk_transaction_dbus_return (context, error);
}