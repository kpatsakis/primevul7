_g_filename_get_random (int         random_part_len,
		        const char *suffix)
{
	const char *letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
	const int   n_letters = strlen (letters);
	int         suffix_len;
	char       *result, *c;
	GRand      *rand;
	int         i;

	suffix_len = suffix != NULL ? strlen (suffix) + 1 : 0;
	result = g_new (char, suffix_len + random_part_len + 1);

	rand = g_rand_new ();
	for (i = 0, c = result; i < random_part_len; i++, c++)
		*c = letters[g_rand_int_range (rand, 0, n_letters)];
	*c = '\0';
	g_rand_free (rand);

	if (suffix != NULL) {
		strcpy (c, ".");
		strcpy (c + 1, suffix);
	}

	return result;
}