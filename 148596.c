pk_transaction_set_hints (PkTransaction *transaction,
			  GVariant *params,
			  GDBusMethodInvocation *context)
{
	gboolean ret;
	guint i;
	g_autofree gchar **hints = NULL;
	g_autoptr(GError) error = NULL;
	g_autofree gchar *dbg = NULL;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	g_variant_get (params, "(^a&s)", &hints);
	dbg = g_strjoinv (", ", (gchar**) hints);
	g_debug ("SetHints method called: %s", dbg);

	/* parse */
	for (i = 0; hints[i] != NULL; i++) {
		g_auto(GStrv) sections = NULL;
		sections = g_strsplit (hints[i], "=", 2);
		if (g_strv_length (sections) == 2) {
			ret = pk_transaction_set_hint (transaction,
						       sections[0],
						       sections[1],
						       &error);
			if (!ret)
				goto out;
		} else {
			g_set_error (&error, PK_TRANSACTION_ERROR,
					     PK_TRANSACTION_ERROR_NOT_SUPPORTED,
					     "Could not parse hint '%s'", hints[i]);
			goto out;
		}
	}
out:
	pk_transaction_dbus_return (context, error);
}