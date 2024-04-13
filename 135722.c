static int scale_linear(int it, int width, int max_change)
{
	/*
	 * make sure that at least one '-' is printed if there were deletions,
	 * and likewise for '+'.
	 */
	if (max_change < 2)
		return it;
	return ((it - 1) * (width - 1) + max_change - 1) / (max_change - 1);
}