pk_transaction_role_to_actions (PkRoleEnum role, guint64 transaction_flags)
{
	const gchar *policy = NULL;
	GPtrArray *result = NULL;
	gboolean check_install_untrusted = FALSE;

	result = g_ptr_array_new_with_free_func (g_free);
	if (result == NULL)
		return result;

	if ((role == PK_ROLE_ENUM_INSTALL_PACKAGES ||
	     role == PK_ROLE_ENUM_INSTALL_FILES ||
	     role == PK_ROLE_ENUM_UPDATE_PACKAGES) &&
	    !pk_bitfield_contain (transaction_flags,
				  PK_TRANSACTION_FLAG_ENUM_ONLY_TRUSTED)) {
		g_ptr_array_add (result, g_strdup ("org.freedesktop.packagekit.package-install-untrusted"));
		check_install_untrusted = TRUE;
	}

	if (role == PK_ROLE_ENUM_INSTALL_PACKAGES &&
	    pk_bitfield_contain (transaction_flags, PK_TRANSACTION_FLAG_ENUM_ALLOW_REINSTALL)) {
		g_ptr_array_add (result, g_strdup ("org.freedesktop.packagekit.package-reinstall"));
	}

	if ((role == PK_ROLE_ENUM_INSTALL_PACKAGES ||
	     role == PK_ROLE_ENUM_UPDATE_PACKAGES) &&
	    pk_bitfield_contain (transaction_flags,
				 PK_TRANSACTION_FLAG_ENUM_ALLOW_DOWNGRADE)) {
		g_ptr_array_add (result, g_strdup ("org.freedesktop.packagekit.package-downgrade"));
	} else if (!check_install_untrusted) {
		switch (role) {
		case PK_ROLE_ENUM_UPDATE_PACKAGES:
			policy = "org.freedesktop.packagekit.system-update";
			break;
		case PK_ROLE_ENUM_INSTALL_SIGNATURE:
			policy = "org.freedesktop.packagekit.system-trust-signing-key";
			break;
		case PK_ROLE_ENUM_REPO_ENABLE:
		case PK_ROLE_ENUM_REPO_SET_DATA:
		case PK_ROLE_ENUM_REPO_REMOVE:
			policy = "org.freedesktop.packagekit.system-sources-configure";
			break;
		case PK_ROLE_ENUM_REFRESH_CACHE:
			policy = "org.freedesktop.packagekit.system-sources-refresh";
			break;
		case PK_ROLE_ENUM_REMOVE_PACKAGES:
			policy = "org.freedesktop.packagekit.package-remove";
			break;
		case PK_ROLE_ENUM_INSTALL_PACKAGES:
		case PK_ROLE_ENUM_INSTALL_FILES:
			policy = "org.freedesktop.packagekit.package-install";
			break;
		case PK_ROLE_ENUM_ACCEPT_EULA:
			policy = "org.freedesktop.packagekit.package-eula-accept";
			break;
		case PK_ROLE_ENUM_CANCEL:
			policy = "org.freedesktop.packagekit.cancel-foreign";
			break;
		case PK_ROLE_ENUM_UPGRADE_SYSTEM:
			policy = "org.freedesktop.packagekit.upgrade-system";
			break;
		case PK_ROLE_ENUM_REPAIR_SYSTEM:
			policy = "org.freedesktop.packagekit.repair-system";
			break;
		default:
			break;
		}
		if (policy != NULL)
			g_ptr_array_add (result, g_strdup (policy));
	}

	return result;
}