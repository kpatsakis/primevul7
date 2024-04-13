pk_transaction_search_check_item (const gchar *values, GError **error)
{
	guint size;

	/* limit to a 1k chunk */
	if (values == NULL) {
		g_set_error_literal (error,
				     PK_TRANSACTION_ERROR,
				     PK_TRANSACTION_ERROR_SEARCH_INVALID,
				     "Search is null. This isn't supposed to happen...");
		return FALSE;
	}
	size = pk_strlen (values, 1024);
	if (size == 0) {
		g_set_error_literal (error,
				     PK_TRANSACTION_ERROR,
				     PK_TRANSACTION_ERROR_SEARCH_INVALID,
				     "Search string zero length");
		return FALSE;
	}
	if (strstr (values, "*") != NULL) {
		g_set_error_literal (error,
				     PK_TRANSACTION_ERROR,
				     PK_TRANSACTION_ERROR_SEARCH_INVALID,
				     "Invalid search containing '*'");
		return FALSE;
	}
	if (strstr (values, "?") != NULL) {
		g_set_error_literal (error,
				     PK_TRANSACTION_ERROR,
				     PK_TRANSACTION_ERROR_SEARCH_INVALID,
				     "Invalid search containing '?'");
		return FALSE;
	}
	if (size == 1024) {
		g_set_error_literal (error,
				     PK_TRANSACTION_ERROR,
				     PK_TRANSACTION_ERROR_SEARCH_INVALID,
				     "The search string length is too large");
		return FALSE;
	}
	return pk_transaction_strvalidate (values, error);
}