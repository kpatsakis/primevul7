free_entry (IconEntry *icon)
{
	g_free (icon->colors);
	g_free (icon->and);
	g_free (icon->xor);
	g_free (icon);
}