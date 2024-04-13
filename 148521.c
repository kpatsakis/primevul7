pk_transaction_make_exclusive (PkTransaction *transaction)
{
	g_return_if_fail (PK_IS_TRANSACTION (transaction));

	g_debug ("changing transaction to exclusive mode");

	transaction->priv->exclusive = TRUE;
}