pk_transaction_status_changed_emit (PkTransaction *transaction, PkStatusEnum status)
{
	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	/* already set */
	if (transaction->priv->status == status)
		return;

	transaction->priv->status = status;

	/* emit */
	pk_transaction_emit_property_changed (transaction,
					      "Status",
					      g_variant_new_uint32 (status));
}