pk_transaction_cancel (PkTransaction *transaction,
		       GVariant *params,
		       GDBusMethodInvocation *context)
{
	gboolean ret;
	const gchar *sender;
	guint uid;
	g_autoptr(GError) error = NULL;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	g_debug ("Cancel method called on %s", transaction->priv->tid);

	/* transaction is already finished */
	if (transaction->priv->state == PK_TRANSACTION_STATE_FINISHED) {
		g_set_error (&error,
			     PK_TRANSACTION_ERROR,
			     PK_TRANSACTION_ERROR_NOT_RUNNING,
			     "Transaction is already finished");
		goto out;
	}

	/* not implemented yet */
	if (!pk_backend_is_implemented (transaction->priv->backend,
					PK_ROLE_ENUM_CANCEL)) {
		g_set_error (&error,
			     PK_TRANSACTION_ERROR,
			     PK_TRANSACTION_ERROR_NOT_SUPPORTED,
			     "Cancel not supported by backend");
		goto out;
	}

	/* if it's finished, cancelling will have no action regardless of uid */
	if (transaction->priv->finished) {
		g_debug ("No point trying to cancel a finished transaction, ignoring");

		/* return from async with success */
		pk_transaction_dbus_return (context, NULL);
		goto out;
	}

	/* check to see if we have an action */
	if (transaction->priv->role == PK_ROLE_ENUM_UNKNOWN) {
		g_set_error (&error,
			     PK_TRANSACTION_ERROR,
			     PK_TRANSACTION_ERROR_NO_ROLE, "No role");
		goto out;
	}

	/* first, check the sender -- if it's the same we don't need to check the uid */
	sender = g_dbus_method_invocation_get_sender (context);
	ret = (g_strcmp0 (transaction->priv->sender, sender) == 0);
	if (ret) {
		g_debug ("same sender, no need to check uid");
		goto skip_uid;
	}

	/* check if we saved the uid */
	if (transaction->priv->uid == PK_TRANSACTION_UID_INVALID) {
		g_set_error (&error,
			     PK_TRANSACTION_ERROR,
			     PK_TRANSACTION_ERROR_CANNOT_CANCEL,
			     "No context from caller to get UID from");
		goto out;
	}

	/* get the UID of the caller */
	uid = pk_dbus_get_uid (transaction->priv->dbus, sender);
	if (uid == PK_TRANSACTION_UID_INVALID) {
		g_set_error (&error,
			     PK_TRANSACTION_ERROR,
			     PK_TRANSACTION_ERROR_INVALID_STATE,
			     "unable to get uid of caller");
		goto out;
	}

	/* check the caller uid with the originator uid */
	if (transaction->priv->uid != uid) {
		g_debug ("uid does not match (%i vs. %i)", transaction->priv->uid, uid);
		ret = pk_transaction_obtain_authorization (transaction,
							   PK_ROLE_ENUM_CANCEL,
							   &error);
		if (!ret)
			goto out;
	}

skip_uid:
	/* if it's never been run, just remove this transaction from the list */
	if (transaction->priv->state <= PK_TRANSACTION_STATE_READY) {
		g_autofree gchar *msg = NULL;
		msg = g_strdup_printf ("%s was cancelled and was never run",
				       transaction->priv->tid);
		pk_transaction_error_code_emit (transaction,
						PK_ERROR_ENUM_TRANSACTION_CANCELLED,
						msg);
		pk_transaction_finished_emit (transaction, PK_EXIT_ENUM_CANCELLED, 0);
		goto out;
	}

	/* set the state, as cancelling might take a few seconds */
	pk_backend_job_set_status (transaction->priv->job, PK_STATUS_ENUM_CANCEL);

	/* we don't want to cancel twice */
	pk_backend_job_set_allow_cancel (transaction->priv->job, FALSE);

	/* we need ::finished to not return success or failed */
	pk_backend_job_set_exit_code (transaction->priv->job, PK_EXIT_ENUM_CANCELLED);

	/* actually run the method */
	pk_backend_cancel (transaction->priv->backend, transaction->priv->job);
out:
	pk_transaction_dbus_return (context, error);
}