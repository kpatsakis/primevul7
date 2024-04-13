_g_strv_prepend (char       **str_array,
		 const char  *str)
{
	char **result;
	int    i;
	int    j;

	result = g_new (char *, g_strv_length (str_array) + 1);
	i = 0;
	result[i++] = g_strdup (str);
	for (j = 0; str_array[j] != NULL; j++)
		result[i++] = g_strdup (str_array[j]);
	result[i] = NULL;

	return result;
}