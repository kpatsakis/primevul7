_g_ptr_array_reverse (GPtrArray *array)
{
	int      i, j;
	gpointer tmp;

	for (i = 0; i < array->len / 2; i++) {
		j = array->len - i - 1;
		tmp = g_ptr_array_index (array, i);
		g_ptr_array_index (array, i) = g_ptr_array_index (array, j);
		g_ptr_array_index (array, j) = tmp;
	}
}