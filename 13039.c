assert_equalfile(typval_T *argvars)
{
    char_u	buf1[NUMBUFLEN];
    char_u	buf2[NUMBUFLEN];
    int		called_emsg_before = called_emsg;
    char_u	*fname1 = tv_get_string_buf_chk(&argvars[0], buf1);
    char_u	*fname2 = tv_get_string_buf_chk(&argvars[1], buf2);
    garray_T	ga;
    FILE	*fd1;
    FILE	*fd2;
    char	line1[200];
    char	line2[200];
    int		lineidx = 0;

    if (called_emsg > called_emsg_before)
	return 0;

    IObuff[0] = NUL;
    fd1 = mch_fopen((char *)fname1, READBIN);
    if (fd1 == NULL)
    {
	vim_snprintf((char *)IObuff, IOSIZE, (char *)e_cant_read_file_str, fname1);
    }
    else
    {
	fd2 = mch_fopen((char *)fname2, READBIN);
	if (fd2 == NULL)
	{
	    fclose(fd1);
	    vim_snprintf((char *)IObuff, IOSIZE, (char *)e_cant_read_file_str, fname2);
	}
	else
	{
	    int	    c1, c2;
	    long    count = 0;
	    long    linecount = 1;

	    for (;;)
	    {
		c1 = fgetc(fd1);
		c2 = fgetc(fd2);
		if (c1 == EOF)
		{
		    if (c2 != EOF)
			STRCPY(IObuff, "first file is shorter");
		    break;
		}
		else if (c2 == EOF)
		{
		    STRCPY(IObuff, "second file is shorter");
		    break;
		}
		else
		{
		    line1[lineidx] = c1;
		    line2[lineidx] = c2;
		    ++lineidx;
		    if (c1 != c2)
		    {
			vim_snprintf((char *)IObuff, IOSIZE,
					    "difference at byte %ld, line %ld",
							     count, linecount);
			break;
		    }
		}
		++count;
		if (c1 == NL)
		{
		    ++linecount;
		    lineidx = 0;
		}
		else if (lineidx + 2 == (int)sizeof(line1))
		{
		    mch_memmove(line1, line1 + 100, lineidx - 100);
		    mch_memmove(line2, line2 + 100, lineidx - 100);
		    lineidx -= 100;
		}
	    }
	    fclose(fd1);
	    fclose(fd2);
	}
    }
    if (IObuff[0] != NUL)
    {
	prepare_assert_error(&ga);
	if (argvars[2].v_type != VAR_UNKNOWN)
	{
	    char_u	numbuf[NUMBUFLEN];
	    char_u	*tofree;

	    ga_concat(&ga, echo_string(&argvars[2], &tofree, numbuf, 0));
	    vim_free(tofree);
	    ga_concat(&ga, (char_u *)": ");
	}
	ga_concat(&ga, IObuff);
	if (lineidx > 0)
	{
	    line1[lineidx] = NUL;
	    line2[lineidx] = NUL;
	    ga_concat(&ga, (char_u *)" after \"");
	    ga_concat(&ga, (char_u *)line1);
	    if (STRCMP(line1, line2) != 0)
	    {
		ga_concat(&ga, (char_u *)"\" vs \"");
		ga_concat(&ga, (char_u *)line2);
	    }
	    ga_concat(&ga, (char_u *)"\"");
	}
	assert_error(&ga);
	ga_clear(&ga);
	return 1;
    }
    return 0;
}