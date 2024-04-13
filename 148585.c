pk_transaction_set_role (PkTransaction *transaction, PkRoleEnum role)
{
	transaction->priv->role = role;

	/* always set transaction exclusive for some actions (improves performance) */
	if (role == PK_ROLE_ENUM_INSTALL_FILES ||
	    role == PK_ROLE_ENUM_INSTALL_PACKAGES ||
	    role == PK_ROLE_ENUM_REMOVE_PACKAGES ||
	    role == PK_ROLE_ENUM_UPDATE_PACKAGES ||
	    role == PK_ROLE_ENUM_UPGRADE_SYSTEM ||
	    role == PK_ROLE_ENUM_REPAIR_SYSTEM) {
		pk_transaction_make_exclusive (transaction);
	}

	pk_transaction_emit_property_changed (transaction,
					      "Role",
					      g_variant_new_uint32 (role));
}