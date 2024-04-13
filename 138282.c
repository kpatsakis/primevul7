_g_mime_type_matches (const char *mime_type,
		      const char *pattern)
{
	return (strcasecmp (mime_type, pattern) == 0);
}