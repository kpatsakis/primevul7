prepare_assert_error(garray_T *gap)
{
    char    buf[NUMBUFLEN];
    char_u  *sname = estack_sfile(ESTACK_NONE);

    ga_init2(gap, 1, 100);
    if (sname != NULL)
    {
	ga_concat(gap, sname);
	if (SOURCING_LNUM > 0)
	    ga_concat(gap, (char_u *)" ");
    }
    if (SOURCING_LNUM > 0)
    {
	sprintf(buf, "line %ld", (long)SOURCING_LNUM);
	ga_concat(gap, (char_u *)buf);
    }
    if (sname != NULL || SOURCING_LNUM > 0)
	ga_concat(gap, (char_u *)": ");
    vim_free(sname);
}