static int __shrink_ple_window(int val, int modifier, int minimum)
{
	if (modifier < 1)
		return ple_window;

	if (modifier < ple_window)
		val /= modifier;
	else
		val -= modifier;

	return max(val, minimum);
}