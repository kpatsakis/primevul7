pk_transaction_skip_auth_checks (PkTransaction *transaction, gboolean skip_checks)
{
	g_return_if_fail (PK_IS_TRANSACTION (transaction));

	transaction->priv->skip_auth_check = skip_checks;
}