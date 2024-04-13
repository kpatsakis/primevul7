pk_transaction_update_detail_cb (PkBackend *backend,
				 PkUpdateDetail *item,
				 PkTransaction *transaction)
{
	const gchar *changelog;
	const gchar *issued;
	const gchar *package_id;
	const gchar *updated;
	const gchar *update_text;
	gchar **bugzilla_urls;
	gchar **cve_urls;
	gchar *empty[] = { NULL };
	gchar **obsoletes;
	gchar **updates;
	gchar **vendor_urls;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	/* add to results */
	pk_results_add_update_detail (transaction->priv->results, item);

	/* emit */
	package_id = pk_update_detail_get_package_id (item);
	updates = pk_update_detail_get_updates (item);
	obsoletes = pk_update_detail_get_obsoletes (item);
	vendor_urls = pk_update_detail_get_vendor_urls (item);
	bugzilla_urls = pk_update_detail_get_bugzilla_urls (item);
	cve_urls = pk_update_detail_get_cve_urls (item);
	update_text = pk_update_detail_get_update_text (item);
	changelog = pk_update_detail_get_changelog (item);
	issued = pk_update_detail_get_issued (item);
	updated = pk_update_detail_get_updated (item);
	g_debug ("emitting update-detail for %s", package_id);
	g_dbus_connection_emit_signal (transaction->priv->connection,
				       NULL,
				       transaction->priv->tid,
				       PK_DBUS_INTERFACE_TRANSACTION,
				       "UpdateDetail",
				       g_variant_new ("(s^as^as^as^as^asussuss)",
						      package_id,
						      updates != NULL ? updates : empty,
						      obsoletes != NULL ? obsoletes : empty,
						      vendor_urls != NULL ? vendor_urls : empty,
						      bugzilla_urls != NULL ? bugzilla_urls : empty,
						      cve_urls != NULL ? cve_urls : empty,
						      pk_update_detail_get_restart (item),
						      update_text != NULL ? update_text : "",
						      changelog != NULL ? changelog : "",
						      pk_update_detail_get_state (item),
						      issued != NULL ? issued : "",
						      updated != NULL ? updated : ""),
				       NULL);
}