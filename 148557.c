pk_transaction_setup_mime_types (PkTransaction *transaction)
{
	guint i;
	g_auto(GStrv) mime_types = NULL;

	/* get list of mime types supported by backends */
	mime_types = pk_backend_get_mime_types (transaction->priv->backend);
	for (i = 0; mime_types[i] != NULL; i++) {
		g_ptr_array_add (transaction->priv->supported_content_types,
				 g_strdup (mime_types[i]));
	}
}