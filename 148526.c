pk_transaction_get_background (PkTransaction *transaction)
{
	g_return_val_if_fail (PK_IS_TRANSACTION (transaction), FALSE);
	return pk_backend_job_get_background (transaction->priv->job);
}