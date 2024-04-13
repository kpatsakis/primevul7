pk_transaction_method_call (GDBusConnection *connection_, const gchar *sender,
			    const gchar *object_path, const gchar *interface_name,
			    const gchar *method_name, GVariant *parameters,
			    GDBusMethodInvocation *invocation, gpointer user_data)
{
	PkTransaction *transaction = PK_TRANSACTION (user_data);

	g_return_if_fail (transaction->priv->sender != NULL);

	/* check is the same as the sender that did CreateTransaction */
	if (g_strcmp0 (transaction->priv->sender, sender) != 0) {
		g_dbus_method_invocation_return_error (invocation,
						       PK_TRANSACTION_ERROR,
						       PK_TRANSACTION_ERROR_REFUSED_BY_POLICY,
						       "sender does not match (%s vs %s)",
						       sender,
						       transaction->priv->sender);
		return;
	}
	if (g_strcmp0 (method_name, "SetHints") == 0) {
		pk_transaction_set_hints (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "AcceptEula") == 0) {
		pk_transaction_accept_eula (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "Cancel") == 0) {
		pk_transaction_cancel (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "DownloadPackages") == 0) {
		pk_transaction_download_packages (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "GetCategories") == 0) {
		pk_transaction_get_categories (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "DependsOn") == 0) {
		pk_transaction_depends_on (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "GetDetails") == 0) {
		pk_transaction_get_details (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "GetDetailsLocal") == 0) {
		pk_transaction_get_details_local (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "GetFilesLocal") == 0) {
		pk_transaction_get_files_local (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "GetFiles") == 0) {
		pk_transaction_get_files (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "GetOldTransactions") == 0) {
		pk_transaction_get_old_transactions (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "GetPackages") == 0) {
		pk_transaction_get_packages (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "GetRepoList") == 0) {
		pk_transaction_get_repo_list (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "RequiredBy") == 0) {
		pk_transaction_required_by (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "GetUpdateDetail") == 0) {
		pk_transaction_get_update_detail (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "GetUpdates") == 0) {
		pk_transaction_get_updates (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "GetDistroUpgrades") == 0) {
		pk_transaction_get_distro_upgrades (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "InstallFiles") == 0) {
		pk_transaction_install_files (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "InstallPackages") == 0) {
		pk_transaction_install_packages (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "InstallSignature") == 0) {
		pk_transaction_install_signature (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "RefreshCache") == 0) {
		pk_transaction_refresh_cache (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "RemovePackages") == 0) {
		pk_transaction_remove_packages (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "RepoEnable") == 0) {
		pk_transaction_repo_enable (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "RepoSetData") == 0) {
		pk_transaction_repo_set_data (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "RepoRemove") == 0) {
		pk_transaction_repo_remove (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "Resolve") == 0) {
		pk_transaction_resolve (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "SearchDetails") == 0) {
		pk_transaction_search_details (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "SearchFiles") == 0) {
		pk_transaction_search_files (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "SearchGroups") == 0) {
		pk_transaction_search_groups (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "SearchNames") == 0) {
		pk_transaction_search_names (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "UpdatePackages") == 0) {
		pk_transaction_update_packages (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "WhatProvides") == 0) {
		pk_transaction_what_provides (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "UpgradeSystem") == 0) {
		pk_transaction_upgrade_system (transaction, parameters, invocation);
		return;
	}
	if (g_strcmp0 (method_name, "RepairSystem") == 0) {
		pk_transaction_repair_system (transaction, parameters, invocation);
		return;
	}

	/* nothing matched */
	g_dbus_method_invocation_return_error (invocation,
					       PK_TRANSACTION_ERROR,
					       PK_TRANSACTION_ERROR_INVALID_STATE,
					       "method from %s not recognised",
					       sender);
}