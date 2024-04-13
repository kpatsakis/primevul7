pk_transaction_get_backend_job (PkTransaction *transaction)
{
	g_return_val_if_fail (PK_IS_TRANSACTION (transaction), NULL);
	return transaction->priv->job;
}