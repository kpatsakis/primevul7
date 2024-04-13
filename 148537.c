pk_transaction_reset_after_lock_error (PkTransaction *transaction)
{
	PkTransactionPrivate *priv = PK_TRANSACTION_GET_PRIVATE (transaction);
	g_return_if_fail (PK_IS_TRANSACTION (transaction));

	/* clear results */
	g_object_unref (priv->results);
	priv->results = pk_results_new ();

	/* reset transaction state */
	/* first set state manually, otherwise set_state will refuse to switch to an earlier stage */
	priv->state = PK_TRANSACTION_STATE_READY;
	pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_READY);

	g_debug ("transaction has been reset after lock-required issue.");
}