pk_transaction_obtain_authorization (PkTransaction *transaction,
				     PkRoleEnum role,
				     GError **error)
{
	g_autoptr(GPtrArray) actions = NULL;
	PkTransactionPrivate *priv = transaction->priv;
	g_autofree gchar *package_ids = NULL;
	g_autoptr(PolkitDetails) details = NULL;
	g_autoptr(GString) string = NULL;

	g_return_val_if_fail (priv->sender != NULL, FALSE);

	/* we don't need to authenticate at all to just download
	 * packages or if we're running unit tests */
	if (pk_bitfield_contain (transaction->priv->cached_transaction_flags,
				 PK_TRANSACTION_FLAG_ENUM_ONLY_DOWNLOAD) ||
			pk_bitfield_contain (transaction->priv->cached_transaction_flags,
				 PK_TRANSACTION_FLAG_ENUM_SIMULATE) ||
			priv->skip_auth_check == TRUE) {
		g_debug ("No authentication required");
		pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_READY);
		return TRUE;
	}

	/* we should always have subject */
	if (priv->subject == NULL) {
		g_set_error (error,
			     PK_TRANSACTION_ERROR,
			     PK_TRANSACTION_ERROR_REFUSED_BY_POLICY,
			     "subject %s not found", priv->sender);
		return FALSE;
	}

	actions = pk_transaction_role_to_actions (role, priv->cached_transaction_flags);
	if (actions == NULL)
		return FALSE;

	return pk_transaction_authorize_actions (transaction, role, actions);
}