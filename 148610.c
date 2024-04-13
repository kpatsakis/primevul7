pk_transaction_speed_cb (PkBackendJob *job,
			 guint speed,
			 PkTransaction *transaction)
{
	/* emit */
	transaction->priv->speed = speed;
	pk_transaction_emit_property_changed (transaction,
					      "Speed",
					      g_variant_new_uint32 (speed));
}