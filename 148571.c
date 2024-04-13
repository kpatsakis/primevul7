pk_transaction_is_finished_with_lock_required (PkTransaction *transaction)
{
	g_autoptr(PkError) error_code = NULL;

	g_return_val_if_fail (PK_IS_TRANSACTION (transaction), FALSE);

	error_code = pk_results_get_error_code (transaction->priv->results);
	if (error_code != NULL &&
	    pk_error_get_code (error_code) == PK_ERROR_ENUM_LOCK_REQUIRED) {
		return TRUE;
	}
	return FALSE;
}