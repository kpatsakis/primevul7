pk_transaction_dispose (GObject *object)
{
	PkTransaction *transaction;

	g_return_if_fail (PK_IS_TRANSACTION (object));

	transaction = PK_TRANSACTION (object);

	/* were we waiting for the client to authorise */
	if (transaction->priv->waiting_for_auth) {
		g_cancellable_cancel (transaction->priv->cancellable);
		/* emit an ::ErrorCode() and then ::Finished() */
		pk_transaction_error_code_emit (transaction, PK_ERROR_ENUM_NOT_AUTHORIZED, "client did not authorize action");
		pk_transaction_finished_emit (transaction, PK_EXIT_ENUM_FAILED, 0);
	}

	if (transaction->priv->registration_id > 0) {
		g_dbus_connection_unregister_object (transaction->priv->connection,
						     transaction->priv->registration_id);
		transaction->priv->registration_id = 0;
	}

	/* send signal to clients that we are about to be destroyed */
	if (transaction->priv->connection != NULL) {
		g_debug ("emitting destroy %s", transaction->priv->tid);
		g_dbus_connection_emit_signal (transaction->priv->connection,
					       NULL,
					       transaction->priv->tid,
					       PK_DBUS_INTERFACE_TRANSACTION,
					       "Destroy",
					       NULL,
					       NULL);
	}

	G_OBJECT_CLASS (pk_transaction_parent_class)->dispose (object);
}