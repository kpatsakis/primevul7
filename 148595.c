pk_transaction_finished_cb (PkBackendJob *job, PkExitEnum exit_enum, PkTransaction *transaction)
{
	guint time_ms;
	guint i;
	PkPackage *item;
	PkInfoEnum info;
	PkBitfield transaction_flags;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	/* have we already been marked as finished? */
	if (transaction->priv->finished) {
		g_warning ("Already finished");
		return;
	}

	/* save this so we know if the cache is valid */
	pk_results_set_exit_code (transaction->priv->results, exit_enum);

	/* don't really finish the transaction if we only completed to wait for lock */
	if (pk_transaction_is_finished_with_lock_required (transaction)) {
		/* finish only for the transaction list */
		g_signal_emit (transaction, signals[SIGNAL_FINISHED], 0);
		return;
	}

	/* handle offline updates */
	transaction_flags = transaction->priv->cached_transaction_flags;
	if (exit_enum == PK_EXIT_ENUM_SUCCESS &&
	    !pk_bitfield_contain (transaction_flags, PK_TRANSACTION_FLAG_ENUM_SIMULATE)) {
		pk_transaction_offline_finished (transaction);
	}

	/* we should get no more from the backend with this tid */
	transaction->priv->finished = TRUE;

	/* if we did ::repo-signature-required or ::eula-required, change the error code */
	if (transaction->priv->emit_signature_required)
		exit_enum = PK_EXIT_ENUM_KEY_REQUIRED;
	else if (transaction->priv->emit_eula_required)
		exit_enum = PK_EXIT_ENUM_EULA_REQUIRED;
	else if (transaction->priv->emit_media_change_required)
		exit_enum = PK_EXIT_ENUM_MEDIA_CHANGE_REQUIRED;

	/* invalidate some caches if we succeeded */
	if (exit_enum == PK_EXIT_ENUM_SUCCESS)
		pk_transaction_finish_invalidate_caches (transaction);

	/* find the length of time we have been running */
	time_ms = pk_transaction_get_runtime (transaction);
	g_debug ("backend was running for %i ms", time_ms);

	/* add to the database if we are going to log it */
	if (transaction->priv->role == PK_ROLE_ENUM_UPDATE_PACKAGES ||
	    transaction->priv->role == PK_ROLE_ENUM_INSTALL_PACKAGES ||
	    transaction->priv->role == PK_ROLE_ENUM_REMOVE_PACKAGES) {
		g_autoptr(GPtrArray) array = NULL;
		g_autofree gchar *packages = NULL;

		array = pk_results_get_package_array (transaction->priv->results);

		/* save to database */
		packages = pk_transaction_package_list_to_string (array);
		if (!pk_strzero (packages))
			pk_transaction_db_set_data (transaction->priv->transaction_db, transaction->priv->tid, packages);

		/* report to syslog */
		for (i = 0; i < array->len; i++) {
			item = g_ptr_array_index (array, i);
			info = pk_package_get_info (item);
			if (info == PK_INFO_ENUM_REMOVING ||
			    info == PK_INFO_ENUM_INSTALLING ||
			    info == PK_INFO_ENUM_UPDATING) {
				syslog (LOG_DAEMON | LOG_DEBUG,
					"in %s for %s package %s was %s for uid %i",
					transaction->priv->tid,
					pk_role_enum_to_string (transaction->priv->role),
					pk_package_get_id (item),
					pk_info_enum_to_string (info),
					transaction->priv->uid);
			}
		}
	}

	/* the repo list will have changed */
	if (transaction->priv->role == PK_ROLE_ENUM_REPO_ENABLE ||
	    transaction->priv->role == PK_ROLE_ENUM_REPO_REMOVE ||
	    transaction->priv->role == PK_ROLE_ENUM_REPO_SET_DATA) {
		pk_backend_repo_list_changed (transaction->priv->backend);
	}

	/* only reset the time if we succeeded */
	if (exit_enum == PK_EXIT_ENUM_SUCCESS)
		pk_transaction_db_action_time_reset (transaction->priv->transaction_db, transaction->priv->role);

	/* did we finish okay? */
	if (exit_enum == PK_EXIT_ENUM_SUCCESS)
		pk_transaction_db_set_finished (transaction->priv->transaction_db, transaction->priv->tid, TRUE, time_ms);
	else
		pk_transaction_db_set_finished (transaction->priv->transaction_db, transaction->priv->tid, FALSE, time_ms);

	/* remove any inhibit */
	//TODO: on main interface

	/* report to syslog */
	if (transaction->priv->uid != PK_TRANSACTION_UID_INVALID) {
		syslog (LOG_DAEMON | LOG_DEBUG,
			"%s transaction %s from uid %i finished with %s after %ims",
			pk_role_enum_to_string (transaction->priv->role),
			transaction->priv->tid,
			transaction->priv->uid,
			pk_exit_enum_to_string (exit_enum),
			time_ms);
	} else {
		syslog (LOG_DAEMON | LOG_DEBUG,
			"%s transaction %s finished with %s after %ims",
			pk_role_enum_to_string (transaction->priv->role),
			transaction->priv->tid,
			pk_exit_enum_to_string (exit_enum),
			time_ms);
	}

	/* this disconnects any pending signals */
	pk_backend_job_disconnect_vfuncs (transaction->priv->job);

	/* destroy the job */
	pk_backend_stop_job (transaction->priv->backend, transaction->priv->job);

	/* we emit last, as other backends will be running very soon after us, and we don't want to be notified */
	pk_transaction_finished_emit (transaction, exit_enum, time_ms);
}