pk_transaction_cancel_bg (PkTransaction *transaction)
{
	g_debug ("CancelBg method called on %s", transaction->priv->tid);

	/* transaction is already finished */
	if (transaction->priv->state == PK_TRANSACTION_STATE_FINISHED)
		return;

	/* not implemented yet */
	if (!pk_backend_is_implemented (transaction->priv->backend,
					PK_ROLE_ENUM_CANCEL)) {
		g_warning ("Cancel not supported by backend");
		return;
	}

	/* if it's never been run, just remove this transaction from the list */
	if (transaction->priv->state <= PK_TRANSACTION_STATE_READY) {
		pk_transaction_finished_emit (transaction, PK_EXIT_ENUM_CANCELLED, 0);
		return;
	}

	/* set the state, as cancelling might take a few seconds */
	pk_backend_job_set_status (transaction->priv->job, PK_STATUS_ENUM_CANCEL);

	/* we don't want to cancel twice */
	pk_backend_job_set_allow_cancel (transaction->priv->job, FALSE);

	/* we need ::finished to not return success or failed */
	pk_backend_job_set_exit_code (transaction->priv->job, PK_EXIT_ENUM_CANCELLED_PRIORITY);

	/* actually run the method */
	pk_backend_cancel (transaction->priv->backend, transaction->priv->job);
}