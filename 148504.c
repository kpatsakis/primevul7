pk_transaction_run (PkTransaction *transaction)
{
	GError *error = NULL;
	PkExitEnum exit_status;
	PkTransactionPrivate *priv = PK_TRANSACTION_GET_PRIVATE (transaction);

	g_return_val_if_fail (PK_IS_TRANSACTION (transaction), FALSE);
	g_return_val_if_fail (priv->tid != NULL, FALSE);
	g_return_val_if_fail (transaction->priv->backend != NULL, FALSE);

	/* we are no longer waiting, we are setting up */
	pk_transaction_status_changed_emit (transaction, PK_STATUS_ENUM_SETUP);

	/* set proxy */
	if (!pk_transaction_set_session_state (transaction, &error)) {
		g_debug ("failed to set the session state (non-fatal): %s",
			 error->message);
		g_clear_error (&error);
	}

	/* already cancelled? */
	if (pk_backend_job_get_exit_code (priv->job) == PK_EXIT_ENUM_CANCELLED) {
		exit_status = pk_backend_job_get_exit_code (priv->job);
		pk_transaction_finished_emit (transaction, exit_status, 0);
		return TRUE;
	}

	/* run the job */
	pk_backend_start_job (priv->backend, priv->job);

	/* is an error code set? */
	if (pk_backend_job_get_is_error_set (priv->job)) {
		exit_status = pk_backend_job_get_exit_code (priv->job);
		pk_transaction_finished_emit (transaction, exit_status, 0);
		/* do not fail the transaction */
	}

	/* check if we should skip this transaction */
	if (pk_backend_job_get_exit_code (priv->job) == PK_EXIT_ENUM_SKIP_TRANSACTION) {
		pk_transaction_finished_emit (transaction, PK_EXIT_ENUM_SUCCESS, 0);
		/* do not fail the transaction */
	}

	/* set the role */
	pk_backend_job_set_role (priv->job, priv->role);
	g_debug ("setting role for %s to %s",
		 priv->tid,
		 pk_role_enum_to_string (priv->role));

	/* reset after the pre-transaction checks */
	pk_backend_job_set_percentage (priv->job, PK_BACKEND_PERCENTAGE_INVALID);

	/* connect signal to receive backend lock changes */
	pk_backend_job_set_vfunc (priv->job,
				  PK_BACKEND_SIGNAL_LOCKED_CHANGED,
				  (PkBackendJobVFunc) pk_transaction_locked_changed_cb,
				  transaction);
	pk_backend_job_set_vfunc (priv->job,
				  PK_BACKEND_SIGNAL_ALLOW_CANCEL,
				  (PkBackendJobVFunc) pk_transaction_allow_cancel_cb,
				  transaction);
	pk_backend_job_set_vfunc (priv->job,
				  PK_BACKEND_SIGNAL_DETAILS,
				  (PkBackendJobVFunc) pk_transaction_details_cb,
				  transaction);
	pk_backend_job_set_vfunc (priv->job,
				  PK_BACKEND_SIGNAL_ERROR_CODE,
				  (PkBackendJobVFunc) pk_transaction_error_code_cb,
				  transaction);
	pk_backend_job_set_vfunc (priv->job,
				  PK_BACKEND_SIGNAL_FILES,
				  (PkBackendJobVFunc) pk_transaction_files_cb,
				  transaction);
	pk_backend_job_set_vfunc (priv->job,
				  PK_BACKEND_SIGNAL_DISTRO_UPGRADE,
				  (PkBackendJobVFunc) pk_transaction_distro_upgrade_cb,
				  transaction);
	pk_backend_job_set_vfunc (priv->job,
				  PK_BACKEND_SIGNAL_FINISHED,
				  (PkBackendJobVFunc) pk_transaction_finished_cb,
				  transaction);
	pk_backend_job_set_vfunc (priv->job,
				  PK_BACKEND_SIGNAL_PACKAGE,
				  (PkBackendJobVFunc) pk_transaction_package_cb,
				  transaction);
	pk_backend_job_set_vfunc (priv->job,
				  PK_BACKEND_SIGNAL_ITEM_PROGRESS,
				  (PkBackendJobVFunc) pk_transaction_item_progress_cb,
				  transaction);
	pk_backend_job_set_vfunc (priv->job,
				  PK_BACKEND_SIGNAL_PERCENTAGE,
				  (PkBackendJobVFunc) pk_transaction_percentage_cb,
				  transaction);
	pk_backend_job_set_vfunc (priv->job,
				  PK_BACKEND_SIGNAL_SPEED,
				  (PkBackendJobVFunc) pk_transaction_speed_cb,
				  transaction);
	pk_backend_job_set_vfunc (priv->job,
				  PK_BACKEND_SIGNAL_DOWNLOAD_SIZE_REMAINING,
				  (PkBackendJobVFunc) pk_transaction_download_size_remaining_cb,
				  transaction);
	pk_backend_job_set_vfunc (priv->job,
				  PK_BACKEND_SIGNAL_REPO_DETAIL,
				  (PkBackendJobVFunc) pk_transaction_repo_detail_cb,
				  transaction);
	pk_backend_job_set_vfunc (priv->job,
				  PK_BACKEND_SIGNAL_REPO_SIGNATURE_REQUIRED,
				  (PkBackendJobVFunc) pk_transaction_repo_signature_required_cb,
				  transaction);
	pk_backend_job_set_vfunc (priv->job,
				  PK_BACKEND_SIGNAL_EULA_REQUIRED,
				  (PkBackendJobVFunc) pk_transaction_eula_required_cb,
				  transaction);
	pk_backend_job_set_vfunc (priv->job,
				  PK_BACKEND_SIGNAL_MEDIA_CHANGE_REQUIRED,
				  (PkBackendJobVFunc) pk_transaction_media_change_required_cb,
				  transaction);
	pk_backend_job_set_vfunc (priv->job,
				  PK_BACKEND_SIGNAL_REQUIRE_RESTART,
				  (PkBackendJobVFunc) pk_transaction_require_restart_cb,
				  transaction);
	pk_backend_job_set_vfunc (priv->job,
				  PK_BACKEND_SIGNAL_STATUS_CHANGED,
				  (PkBackendJobVFunc) pk_transaction_status_changed_cb,
				  transaction);
	pk_backend_job_set_vfunc (priv->job,
				  PK_BACKEND_SIGNAL_UPDATE_DETAIL,
				  (PkBackendJobVFunc) pk_transaction_update_detail_cb,
				  transaction);
	pk_backend_job_set_vfunc (priv->job,
				  PK_BACKEND_SIGNAL_CATEGORY,
				  (PkBackendJobVFunc) pk_transaction_category_cb,
				  transaction);

	/* do the correct action with the cached parameters */
	switch (priv->role) {
	case PK_ROLE_ENUM_DEPENDS_ON:
		pk_backend_depends_on (priv->backend,
					priv->job,
					priv->cached_filters,
					priv->cached_package_ids,
					priv->cached_force);
		break;
	case PK_ROLE_ENUM_GET_UPDATE_DETAIL:
		pk_backend_get_update_detail (priv->backend,
					      priv->job,
					      priv->cached_package_ids);
		break;
	case PK_ROLE_ENUM_RESOLVE:
		pk_backend_resolve (priv->backend,
				    priv->job,
				    priv->cached_filters,
				    priv->cached_package_ids);
		break;
	case PK_ROLE_ENUM_DOWNLOAD_PACKAGES:
		pk_backend_download_packages (priv->backend,
					      priv->job,
					      priv->cached_package_ids,
					      priv->cached_directory);
		break;
	case PK_ROLE_ENUM_GET_DETAILS:
		pk_backend_get_details (priv->backend,
					priv->job,
					priv->cached_package_ids);
		break;
	case PK_ROLE_ENUM_GET_DETAILS_LOCAL:
		pk_backend_get_details_local (priv->backend,
					      priv->job,
					      priv->cached_full_paths);
		break;
	case PK_ROLE_ENUM_GET_FILES_LOCAL:
		pk_backend_get_files_local (priv->backend,
					    priv->job,
					    priv->cached_full_paths);
		break;
	case PK_ROLE_ENUM_GET_DISTRO_UPGRADES:
		pk_backend_get_distro_upgrades (priv->backend,
						priv->job);
		break;
	case PK_ROLE_ENUM_GET_FILES:
		pk_backend_get_files (priv->backend,
				      priv->job,
				      priv->cached_package_ids);
		break;
	case PK_ROLE_ENUM_REQUIRED_BY:
		pk_backend_required_by (priv->backend,
					 priv->job,
					 priv->cached_filters,
					 priv->cached_package_ids,
					 priv->cached_force);
		break;
	case PK_ROLE_ENUM_WHAT_PROVIDES:
		pk_backend_what_provides (priv->backend,
					  priv->job,
					  priv->cached_filters,
					  priv->cached_values);
		break;
	case PK_ROLE_ENUM_GET_UPDATES:
		pk_backend_get_updates (priv->backend,
					priv->job,
					priv->cached_filters);
		break;
	case PK_ROLE_ENUM_GET_PACKAGES:
		pk_backend_get_packages (priv->backend,
					 priv->job,
					 priv->cached_filters);
		break;
	case PK_ROLE_ENUM_SEARCH_DETAILS:
		pk_backend_search_details (priv->backend,
					   priv->job,
					   priv->cached_filters,
					   priv->cached_values);
		break;
	case PK_ROLE_ENUM_SEARCH_FILE:
		pk_backend_search_files (priv->backend,
					 priv->job,
					 priv->cached_filters,
					 priv->cached_values);
		break;
	case PK_ROLE_ENUM_SEARCH_GROUP:
		pk_backend_search_groups (priv->backend,
					  priv->job,
					  priv->cached_filters,
					  priv->cached_values);
		break;
	case PK_ROLE_ENUM_SEARCH_NAME:
		pk_backend_search_names (priv->backend,
					 priv->job,
					 priv->cached_filters,
					 priv->cached_values);
		break;
	case PK_ROLE_ENUM_INSTALL_PACKAGES:
		pk_backend_install_packages (priv->backend,
					     priv->job,
					     priv->cached_transaction_flags,
					     priv->cached_package_ids);
		break;
	case PK_ROLE_ENUM_INSTALL_FILES:
		pk_backend_install_files (priv->backend,
					  priv->job,
					  priv->cached_transaction_flags,
					  priv->cached_full_paths);
		break;
	case PK_ROLE_ENUM_INSTALL_SIGNATURE:
		pk_backend_install_signature (priv->backend,
					      priv->job,
					      PK_SIGTYPE_ENUM_GPG,
					      priv->cached_key_id,
					      priv->cached_package_id);
		break;
	case PK_ROLE_ENUM_REFRESH_CACHE:
		pk_backend_refresh_cache (priv->backend,
					  priv->job,
					  priv->cached_force);
		break;
	case PK_ROLE_ENUM_REMOVE_PACKAGES:
		pk_backend_remove_packages (priv->backend,
					    priv->job,
					    priv->cached_transaction_flags,
					    priv->cached_package_ids,
					    priv->cached_allow_deps,
					    priv->cached_autoremove);
		break;
	case PK_ROLE_ENUM_UPDATE_PACKAGES:
		pk_backend_update_packages (priv->backend,
					    priv->job,
					    priv->cached_transaction_flags,
					    priv->cached_package_ids);
		break;
	case PK_ROLE_ENUM_GET_CATEGORIES:
		pk_backend_get_categories (priv->backend,
					   priv->job);
		break;
	case PK_ROLE_ENUM_GET_REPO_LIST:
		pk_backend_get_repo_list (priv->backend,
					  priv->job,
					  priv->cached_filters);
		break;
	case PK_ROLE_ENUM_REPO_ENABLE:
		pk_backend_repo_enable (priv->backend,
					priv->job,
					priv->cached_repo_id,
					priv->cached_enabled);
		break;
	case PK_ROLE_ENUM_REPO_SET_DATA:
		pk_backend_repo_set_data (priv->backend,
					  priv->job,
					  priv->cached_repo_id,
					  priv->cached_parameter,
					  priv->cached_value);
		break;
	case PK_ROLE_ENUM_REPO_REMOVE:
		pk_backend_repo_remove (priv->backend,
					priv->job,
					priv->cached_transaction_flags,
					priv->cached_repo_id,
					priv->cached_autoremove);
		break;
	case PK_ROLE_ENUM_UPGRADE_SYSTEM:
		pk_backend_upgrade_system (priv->backend,
					   priv->job,
					   priv->cached_transaction_flags,
					   priv->cached_value,
					   priv->cached_upgrade_kind);
		break;
	case PK_ROLE_ENUM_REPAIR_SYSTEM:
		pk_backend_repair_system (priv->backend,
					  priv->job,
					  priv->cached_transaction_flags);
		break;
	/* handled in the engine without a transaction */
	case PK_ROLE_ENUM_CANCEL:
	case PK_ROLE_ENUM_GET_OLD_TRANSACTIONS:
	case PK_ROLE_ENUM_ACCEPT_EULA:
		g_warning ("role %s should be handled by engine",
			   pk_role_enum_to_string (priv->role));
		break;
	default:
		g_error ("failed to run as role not assigned");
		return FALSE;
		break;
	}
	return TRUE;
}