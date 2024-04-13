static int __grow_ple_window(int val)
{
	if (ple_window_grow < 1)
		return ple_window;

	val = min(val, ple_window_actual_max);

	if (ple_window_grow < ple_window)
		val *= ple_window_grow;
	else
		val += ple_window_grow;

	return val;
}