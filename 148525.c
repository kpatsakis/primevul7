pk_transaction_category_cb (PkBackendJob *job,
			    PkCategory *item,
			    PkTransaction *transaction)
{
	g_autofree gchar *parent_id = NULL;
	g_autofree gchar *cat_id = NULL;
	g_autofree gchar *name = NULL;
	g_autofree gchar *summary = NULL;
	g_autofree gchar *icon = NULL;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	/* add to results */
	pk_results_add_category (transaction->priv->results, item);

	/* get data */
	g_object_get (item,
		      "parent-id", &parent_id,
		      "cat-id", &cat_id,
		      "name", &name,
		      "summary", &summary,
		      "icon", &icon,
		      NULL);

	/* emit */
	g_debug ("emitting category %s, %s, %s, %s, %s ", parent_id, cat_id, name, summary, icon);
	g_dbus_connection_emit_signal (transaction->priv->connection,
				       NULL,
				       transaction->priv->tid,
				       PK_DBUS_INTERFACE_TRANSACTION,
				       "Category",
				       g_variant_new ("(sssss)",
						      parent_id != NULL ? parent_id : "",
						      cat_id,
						      name,
						      summary,
						      icon != NULL ? icon : ""),
				       NULL);
}