_g_ptr_array_copy (GPtrArray *array)
{
	GPtrArray *new_array;

	if (array == NULL)
		return NULL;

	new_array = g_ptr_array_sized_new (array->len);
	memcpy (new_array->pdata, array->pdata, array->len * sizeof (gpointer));
	new_array->len = array->len;

	return new_array;
}