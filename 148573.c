pk_transaction_locked_changed_cb (PkBackendJob *job,
				gboolean locked,
				PkTransaction *transaction)
{
	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	g_debug ("backend job lock status changed: %i", locked);

	/* if backend cache is locked at some time, this transaction is running in exclusive mode */
	if (locked)
		pk_transaction_make_exclusive (transaction);
}