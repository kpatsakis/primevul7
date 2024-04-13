_g_key_file_get_string_list (GKeyFile    *key_file,
			     const char  *group_name,
			     const char  *key,
			     GError    **error)
{
	char  **strv;
	GList  *list;
	int     i;

	strv = g_key_file_get_string_list (key_file, group_name, key, NULL, error);
	if (strv == NULL)
		return NULL;

	list = NULL;
	for (i = 0; strv[i] != NULL; i++)
		list = g_list_prepend (list, strv[i]);

	g_free (strv);

	return g_list_reverse (list);
}