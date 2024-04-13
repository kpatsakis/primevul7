_g_strv_remove (char       **str_array,
		const char  *str)
{
	int i;
	int j;

	if (str == NULL)
		return FALSE;

	for (i = 0; str_array[i] != NULL; i++)
		if (strcmp (str_array[i], str) == 0)
			break;

	if (str_array[i] == NULL)
		return FALSE;

	for (j = i; str_array[j] != NULL; j++)
		str_array[j] = str_array[j + 1];

	return TRUE;
}