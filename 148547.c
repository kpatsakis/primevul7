pk_transaction_download_size_remaining_cb (PkBackendJob *job,
					   guint64 *download_size_remaining,
					   PkTransaction *transaction)
{
	/* emit */
	transaction->priv->download_size_remaining = *download_size_remaining;
	pk_transaction_emit_property_changed (transaction,
					      "DownloadSizeRemaining",
					      g_variant_new_uint64 (*download_size_remaining));
}