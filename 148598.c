pk_transaction_get_old_transactions (PkTransaction *transaction,
				     GVariant *params,
				     GDBusMethodInvocation *context)
{
	const gchar *cmdline;
	const gchar *data;
	const gchar *modified;
	const gchar *tid;
	gboolean succeeded;
	GList *l;
	GList *transactions = NULL;
	guint duration;
	guint idle_id;
	guint number;
	guint uid;
	PkRoleEnum role;
	PkTransactionPast *item;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	g_variant_get (params, "(u)",
		       &number);

	g_debug ("GetOldTransactions method called");

	pk_transaction_set_role (transaction, PK_ROLE_ENUM_GET_OLD_TRANSACTIONS);
	transactions = pk_transaction_db_get_list (transaction->priv->transaction_db, number);
	for (l = transactions; l != NULL; l = l->next) {
		item = PK_TRANSACTION_PAST (l->data);

		/* add to results */
		pk_results_add_transaction (transaction->priv->results, item);

		/* get data */
		role = pk_transaction_past_get_role (item);
		tid = pk_transaction_past_get_id (item);
		modified = pk_transaction_past_get_timespec (item);
		succeeded = pk_transaction_past_get_succeeded (item);
		duration = pk_transaction_past_get_duration (item);
		data = pk_transaction_past_get_data (item);
		uid = pk_transaction_past_get_uid (item);
		cmdline = pk_transaction_past_get_cmdline (item);

		/* emit */
		g_debug ("adding transaction %s, %s, %i, %s, %i, %s, %i, %s",
			 tid, modified, succeeded,
			 pk_role_enum_to_string (role),
			 duration, data, uid, cmdline);
		g_dbus_connection_emit_signal (transaction->priv->connection,
					       NULL,
					       transaction->priv->tid,
					       PK_DBUS_INTERFACE_TRANSACTION,
					       "Transaction",
					       g_variant_new ("(osbuusus)",
							      tid,
							      modified,
							      succeeded,
							      role,
							      duration,
							      data != NULL ? data : "",
							      uid,
							      cmdline != NULL ? cmdline : ""),
					       NULL);
	}
	g_list_free_full (transactions, (GDestroyNotify) g_object_unref);

	idle_id = g_idle_add ((GSourceFunc) pk_transaction_finished_idle_cb, transaction);
	g_source_set_name_by_id (idle_id, "[PkTransaction] finished from get-old-transactions");

	pk_transaction_dbus_return (context, NULL);
}