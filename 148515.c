pk_transaction_is_exclusive (PkTransaction *transaction)
{
	g_return_val_if_fail (PK_IS_TRANSACTION (transaction), FALSE);

	return transaction->priv->exclusive;
}