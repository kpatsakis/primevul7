pk_transaction_package_list_to_string (GPtrArray *array)
{
	GString *string;
	PkPackage *pkg;
	guint i;

	string = g_string_new ("");
	for (i = 0; i < array->len; i++) {
		pkg = g_ptr_array_index (array, i);
		g_string_append_printf (string, "%s\t%s\t%s\n",
					pk_info_enum_to_string (pk_package_get_info (pkg)),
					pk_package_get_id (pkg),
					pk_package_get_summary (pkg));
	}

	/* remove trailing newline */
	if (string->len != 0)
		g_string_set_size (string, string->len-1);
	return g_string_free (string, FALSE);
}