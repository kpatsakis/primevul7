static void update_ple_window_actual_max(void)
{
	ple_window_actual_max =
			__shrink_ple_window(max(ple_window_max, ple_window),
			                    ple_window_grow, INT_MIN);
}