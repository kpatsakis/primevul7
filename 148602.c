pk_transaction_search_check (gchar **values, GError **error)
{
	guint i;

	/* check each parameter */
	for (i = 0; values[i] != NULL; i++) {
		if (!pk_transaction_search_check_item (values[i], error))
			return FALSE;
	}
	return TRUE;
}