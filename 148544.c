pk_transaction_finish_invalidate_caches (PkTransaction *transaction)
{
	PkTransactionPrivate *priv = transaction->priv;

	g_return_val_if_fail (PK_IS_TRANSACTION (transaction), FALSE);

	/* could the update list have changed? */
	if (pk_bitfield_contain (transaction->priv->cached_transaction_flags,
				  PK_TRANSACTION_FLAG_ENUM_SIMULATE))
		goto out;
	if (pk_bitfield_contain (transaction->priv->cached_transaction_flags,
				  PK_TRANSACTION_FLAG_ENUM_ONLY_DOWNLOAD))
		goto out;
	if (priv->role == PK_ROLE_ENUM_UPDATE_PACKAGES ||
	    priv->role == PK_ROLE_ENUM_INSTALL_PACKAGES ||
	    priv->role == PK_ROLE_ENUM_REMOVE_PACKAGES ||
	    priv->role == PK_ROLE_ENUM_REPO_ENABLE ||
	    priv->role == PK_ROLE_ENUM_REPO_SET_DATA ||
	    priv->role == PK_ROLE_ENUM_REPO_REMOVE ||
	    priv->role == PK_ROLE_ENUM_REFRESH_CACHE) {

		/* this needs to be done after a small delay */
		pk_backend_updates_changed_delay (priv->backend,
						  PK_TRANSACTION_UPDATES_CHANGED_TIMEOUT);
	}
out:
	return TRUE;
}