_g_string_list_free (GList *path_list)
{
	if (path_list == NULL)
		return;
	g_list_foreach (path_list, (GFunc) g_free, NULL);
	g_list_free (path_list);
}