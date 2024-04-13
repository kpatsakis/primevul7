pk_transaction_percentage_cb (PkBackendJob *job,
			      guint percentage,
			      PkTransaction *transaction)
{
	/* emit */
	transaction->priv->percentage = percentage;
	pk_transaction_emit_property_changed (transaction,
					      "Percentage",
					      g_variant_new_uint32 (percentage));
}