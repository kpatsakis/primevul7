pk_transaction_vanished_cb (GDBusConnection *connection,
			    const gchar *name,
			    gpointer user_data)
{
	PkTransaction *transaction = PK_TRANSACTION (user_data);

	g_return_if_fail (PK_IS_TRANSACTION (transaction));

	transaction->priv->caller_active = FALSE;

	/* emit */
	pk_transaction_emit_property_changed (transaction,
					      "CallerActive",
					      g_variant_new_boolean (transaction->priv->caller_active));
}