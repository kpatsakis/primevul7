pk_transaction_details_cb (PkBackendJob *job,
			   PkDetails *item,
			   PkTransaction *transaction)
{
	GVariantBuilder builder;
	PkGroupEnum group;
	const gchar *tmp;
	guint64 size;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	/* add to results */
	pk_results_add_details (transaction->priv->results, item);

	/* emit */
	g_debug ("emitting details");
	g_variant_builder_init (&builder, G_VARIANT_TYPE("a{sv}"));
	g_variant_builder_add (&builder, "{sv}", "package-id",
			       g_variant_new_string (pk_details_get_package_id (item)));
	group = pk_details_get_group (item);
	if (group != PK_GROUP_ENUM_UNKNOWN)
		g_variant_builder_add (&builder, "{sv}", "group",
				       g_variant_new_uint32 (group));
	tmp = pk_details_get_summary (item);
	if (tmp != NULL)
		g_variant_builder_add (&builder, "{sv}", "summary",
				       g_variant_new_string (tmp));
	tmp = pk_details_get_description (item);
	if (tmp != NULL)
		g_variant_builder_add (&builder, "{sv}", "description",
				       g_variant_new_string (tmp));
	tmp = pk_details_get_url (item);
	if (tmp != NULL)
		g_variant_builder_add (&builder, "{sv}", "url",
				       g_variant_new_string (tmp));
	tmp = pk_details_get_license (item);
	if (tmp != NULL)
		g_variant_builder_add (&builder, "{sv}", "license",
				       g_variant_new_string (tmp));
	size = pk_details_get_size (item);
	if (size != 0)
		g_variant_builder_add (&builder, "{sv}", "size",
				       g_variant_new_uint64 (size));

	g_dbus_connection_emit_signal (transaction->priv->connection,
				       NULL,
				       transaction->priv->tid,
				       PK_DBUS_INTERFACE_TRANSACTION,
				       "Details",
				       g_variant_new ("(a{sv})", &builder),
				       NULL);
}