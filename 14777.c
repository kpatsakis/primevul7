source_all_matches(char_u *pat)
{
    int	    num_files;
    char_u  **files;
    int	    i;

    if (gen_expand_wildcards(1, &pat, &num_files, &files, EW_FILE) == OK)
    {
	for (i = 0; i < num_files; ++i)
	    (void)do_source(files[i], FALSE, DOSO_NONE, NULL);
	FreeWild(num_files, files);
    }
}