pk_transaction_error_code_emit (PkTransaction *transaction,
				PkErrorEnum error_enum,
				const gchar *details)
{
	g_debug ("emitting error-code %s, '%s'",
		 pk_error_enum_to_string (error_enum),
		 details);
	g_dbus_connection_emit_signal (transaction->priv->connection,
				       NULL,
				       transaction->priv->tid,
				       PK_DBUS_INTERFACE_TRANSACTION,
				       "ErrorCode",
				       g_variant_new ("(us)",
						      error_enum,
						      details),
				       NULL);
}