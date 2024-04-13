pk_transaction_get_state (PkTransaction *transaction)
{
	g_return_val_if_fail (PK_IS_TRANSACTION (transaction), PK_TRANSACTION_STATE_UNKNOWN);

	return transaction->priv->state;
}