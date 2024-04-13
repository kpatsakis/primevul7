pk_transaction_is_supported_content_type (PkTransaction *transaction,
					  const gchar *content_type)
{
	const gchar *tmp;
	GPtrArray *array = transaction->priv->supported_content_types;
	guint i;

	/* can we support this one? */
	for (i = 0; i < array->len; i++) {
		tmp = g_ptr_array_index (array, i);
		if (g_strcmp0 (tmp, content_type) == 0)
			return TRUE;
	}
	return FALSE;
}