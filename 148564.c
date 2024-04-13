pk_transaction_eula_required_cb (PkBackend *backend,
				 PkEulaRequired *item,
				 PkTransaction *transaction)
{
	const gchar *eula_id;
	const gchar *package_id;
	const gchar *vendor_name;
	const gchar *license_agreement;

	g_return_if_fail (PK_IS_TRANSACTION (transaction));
	g_return_if_fail (transaction->priv->tid != NULL);

	/* add to results */
	pk_results_add_eula_required (transaction->priv->results, item);

	/* get data */
	eula_id = pk_eula_required_get_eula_id (item);
	package_id = pk_eula_required_get_package_id (item);
	vendor_name = pk_eula_required_get_vendor_name (item);
	license_agreement = pk_eula_required_get_license_agreement (item);

	/* emit */
	g_debug ("emitting eula-required %s, %s, %s, %s",
		   eula_id, package_id, vendor_name, license_agreement);
	g_dbus_connection_emit_signal (transaction->priv->connection,
				       NULL,
				       transaction->priv->tid,
				       PK_DBUS_INTERFACE_TRANSACTION,
				       "EulaRequired",
				       g_variant_new ("(ssss)",
						      eula_id,
						      package_id,
						      vendor_name != NULL ? vendor_name : "",
						      license_agreement != NULL ? license_agreement : ""),
				       NULL);

	/* we should mark this transaction so that we finish with a special code */
	transaction->priv->emit_eula_required = TRUE;
}