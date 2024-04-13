_g_object_list_ref (GList *list)
{
	GList *new_list;

	if (list == NULL)
		return NULL;

	new_list = g_list_copy (list);
	g_list_foreach (new_list, (GFunc) g_object_ref, NULL);

	return new_list;
}