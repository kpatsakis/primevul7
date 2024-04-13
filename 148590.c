pk_transaction_authorize_actions (PkTransaction *transaction,
				  PkRoleEnum role,
				  GPtrArray *actions)
{
	const gchar *action_id = NULL;
	g_autoptr(PolkitDetails) details = NULL;
	g_autofree gchar *package_ids = NULL;
	GString *string = NULL;
	PkTransactionPrivate *priv = transaction->priv;
	const gchar *text = NULL;
	struct AuthorizeActionsData *data = NULL;

	if (actions->len <= 0) {
		g_debug ("No authentication required");
		pk_transaction_set_state (transaction, PK_TRANSACTION_STATE_READY);
		return TRUE;
	}
	action_id = g_ptr_array_index (actions, 0);

	/* log */
	syslog (LOG_AUTH | LOG_INFO,
		"uid %i is trying to obtain %s auth (only_trusted:%i)",
		priv->uid,
		action_id,
		pk_bitfield_contain (priv->cached_transaction_flags,
					PK_TRANSACTION_FLAG_ENUM_ONLY_TRUSTED));

	/* set transaction state */
	pk_transaction_set_state (transaction,
				  PK_TRANSACTION_STATE_WAITING_FOR_AUTH);

	/* check subject */
	priv->waiting_for_auth = TRUE;

	/* insert details about the authorization */
	details = polkit_details_new ();

	/* do we have package details? */
	if (priv->cached_package_id != NULL)
		package_ids = g_strdup (priv->cached_package_id);
	else if (priv->cached_package_ids != NULL)
		package_ids = pk_package_ids_to_string (priv->cached_package_ids);

	/* save optional stuff */
	if (package_ids != NULL)
		polkit_details_insert (details, "package_ids", package_ids);
	if (priv->cmdline != NULL)
		polkit_details_insert (details, "cmdline", priv->cmdline);

	/* do not use the default icon and wording for some roles */
	if ((role == PK_ROLE_ENUM_INSTALL_PACKAGES ||
	    role == PK_ROLE_ENUM_UPDATE_PACKAGES) &&
	    !pk_bitfield_contain (priv->cached_transaction_flags,
				  PK_TRANSACTION_FLAG_ENUM_ONLY_TRUSTED)) {

		/* don't use the friendly PackageKit icon as this is
		 * might be a ricky authorisation */
		polkit_details_insert (details, "polkit.icon_name", "emblem-important");

		string = g_string_new ("");

		/* TRANSLATORS: is not GPG signed */
		g_string_append (string, g_dgettext (GETTEXT_PACKAGE, N_("The software is not from a trusted source.")));
		g_string_append (string, "\n");

		/* UpdatePackages */
		if (priv->role == PK_ROLE_ENUM_UPDATE_PACKAGES) {

			/* TRANSLATORS: user has to trust provider -- I know, this sucks */
			text = g_dngettext (GETTEXT_PACKAGE,
					    N_("Do not update this package unless you are sure it is safe to do so."),
					    N_("Do not update these packages unless you are sure it is safe to do so."),
					    g_strv_length (priv->cached_package_ids));
			g_string_append (string, text);
		}

		/* InstallPackages */
		if (priv->role == PK_ROLE_ENUM_INSTALL_PACKAGES) {

			/* TRANSLATORS: user has to trust provider -- I know, this sucks */
			text = g_dngettext (GETTEXT_PACKAGE,
					    N_("Do not install this package unless you are sure it is safe to do so."),
					    N_("Do not install these packages unless you are sure it is safe to do so."),
					    g_strv_length (priv->cached_package_ids));
			g_string_append (string, text);
		}
		if (string->len > 0) {
			polkit_details_insert (details, "polkit.gettext_domain", GETTEXT_PACKAGE);
			polkit_details_insert (details, "polkit.message", string->str);
		}
	}

	data = g_new (struct AuthorizeActionsData, 1);
	data->transaction = transaction;
	data->role = role;
	data->actions = g_ptr_array_ref (actions);

	g_debug ("authorizing action %s", action_id);
	/* do authorization async */
	polkit_authority_check_authorization (priv->authority,
					      priv->subject,
					      action_id,
					      details,
					      POLKIT_CHECK_AUTHORIZATION_FLAGS_ALLOW_USER_INTERACTION,
					      priv->cancellable,
					      (GAsyncReadyCallback) pk_transaction_authorize_actions_finished_cb,
					      data);
	return TRUE;
}