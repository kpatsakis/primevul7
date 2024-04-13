pk_transaction_strvalidate (const gchar *text, GError **error)
{
	guint length;

	/* maximum size is 1024 */
	length = pk_strlen (text, 1024);
	if (length == 0) {
		g_set_error_literal (error,
				     PK_TRANSACTION_ERROR,
				     PK_TRANSACTION_ERROR_INPUT_INVALID,
				     "Invalid input passed to daemon: zero length string");
		return FALSE;
	}
	if (length > 1024) {
		g_set_error (error, PK_TRANSACTION_ERROR, PK_TRANSACTION_ERROR_INPUT_INVALID,
			     "Invalid input passed to daemon: input too long: %u", length);
		return FALSE;
	}

	/* just check for valid UTF-8 */
	if (!g_utf8_validate (text, -1, NULL)) {
		g_set_error (error,
			     PK_TRANSACTION_ERROR,
			     PK_TRANSACTION_ERROR_INPUT_INVALID,
			     "Invalid input passed to daemon: %s", text);
		return FALSE;
	}
	return TRUE;
}