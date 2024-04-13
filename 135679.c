const char *diff_get_color(int diff_use_color, enum color_diff ix)
{
	if (diff_use_color)
		return diff_colors[ix];
	return "";
}