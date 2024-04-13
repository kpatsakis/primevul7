_g_ptr_array_free_full (GPtrArray *array,
                        GFunc      free_func,
                        gpointer   user_data)
{
	g_ptr_array_foreach (array, free_func, user_data);
	g_ptr_array_free (array, TRUE);
}