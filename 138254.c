_g_ptr_array_binary_search (GPtrArray    *array,
			   gpointer      value,
			   GCompareFunc  func)
{
	int l, r, p, cmp = -1;

	l = 0;
	r = array->len;
	while (l < r) {
		p = l + ((r - l) / 2);
		cmp = func(value, &g_ptr_array_index (array, p));
		if (cmp == 0)
			return p;
		else if (cmp < 0)
			r = p;
		else
			l = p + 1;
	}

	return -1;
}