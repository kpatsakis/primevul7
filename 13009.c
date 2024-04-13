read_double(gerb_file_t *fd, number_fmt_t fmt, gerbv_omit_zeros_t omit_zeros, int decimals)
{
    int read;
    char temp[DRILL_READ_DOUBLE_SIZE];
    unsigned int i = 0, ndigits = 0;
    double result;
    gboolean decimal_point = FALSE;
    gboolean sign_prepend = FALSE;

    memset(temp, 0, sizeof(temp));

    read = gerb_fgetc(fd);
    while(read != EOF && i < (DRILL_READ_DOUBLE_SIZE -1) &&
	  (isdigit(read) || read == '.' || read == ',' || read == '+' || read == '-')) {
      if(read == ',' || read == '.') decimal_point = TRUE;
      
      /*
       * FIXME -- if we are going to do this, don't we need a
       * locale-independent strtod()?  I think pcb has one.
       */
      if(read == ',')
	    read = '.'; /* adjust for strtod() */
      
      if(isdigit(read)) ndigits++;
      
	if(read == '-' || read == '+')
	    sign_prepend = TRUE;

      temp[i++] = (char)read;
      read = gerb_fgetc(fd);
    }

    temp[i] = 0;
    gerb_ungetc(fd);

    if (decimal_point) {
	result = strtod(temp, NULL);
    } else {
	unsigned int wantdigits;
	double scale;
	char tmp2[DRILL_READ_DOUBLE_SIZE];

	memset(tmp2, 0, sizeof(tmp2));

	/* Nothing to take care for when leading zeros are
	   omitted. */
	if (omit_zeros == GERBV_OMIT_ZEROS_TRAILING) {
	    switch (fmt) {
	    case FMT_00_0000:
	      wantdigits = 2;
	      break;

	    case FMT_000_000:
	      wantdigits = 3;
	      break;

	    case FMT_0000_00:
		wantdigits = 4;
		break;

	    case FMT_000_00:
		wantdigits = 3;
		break;

	    case FMT_USER:
		wantdigits = decimals;
		break;

	    default:
	      /* cannot happen, just plugs a compiler warning */
	      fprintf(stderr, _("%s():  omit_zeros == GERBV_OMIT_ZEROS_TRAILING but fmt = %d.\n"
		      "This should never have happened\n"), __FUNCTION__, fmt);
	      return 0;
	    }
	    
	    /* need to add an extra char for '+' or '-' */
	    if (sign_prepend)
	      wantdigits++;


	    /* 
	     * we need at least wantdigits + one for the decimal place
	     * + one for the terminating null character
	     */
	    if (wantdigits > sizeof(tmp2) - 2) {
	      fprintf(stderr, _("%s():  wantdigits = %d which exceeds the maximum allowed size\n"),
		      __FUNCTION__, wantdigits);
	      return 0;
	    }

	    /*
	     * After we have read the correct number of digits
	     * preceeding the decimal point, insert a decimal point
	     * and append the rest of the digits.
	     */
	    dprintf("%s():  wantdigits = %d, strlen(\"%s\") = %ld\n",
		    __FUNCTION__, wantdigits, temp, (long) strlen(temp));
	    for (i = 0 ; i < wantdigits && i < strlen(temp) ; i++) {
	      tmp2[i] = temp[i];
	    }
	    for ( ; i < wantdigits ; i++) {
	      tmp2[i] = '0';
	    }
	    tmp2[i++] = '.';
	    for ( ; i <= strlen(temp) ; i++) {
	      tmp2[i] = temp[i-1];
	    }
	    dprintf("%s():  After dealing with trailing zero suppression, convert \"%s\"\n", __FUNCTION__, tmp2);
	    scale = 1.0;
	    
	    for (i = 0 ; i <= strlen(tmp2) && i < sizeof (temp) ; i++) {
	      temp[i] = tmp2[i];
	    }

	} else {

	  /*
	   * figure out the scale factor when we are not suppressing
	   * trailing zeros.
	   */
	  switch (fmt) {
	  case FMT_00_0000:
	    scale = 1E-4;
	    break;
	    
	  case FMT_000_000:
	    scale = 1E-3;
	    break;
	    
	  case FMT_000_00:
	  case FMT_0000_00:
	    scale = 1E-2;
	    break;
	    
	  case FMT_USER:
	    scale = pow (10.0, -1.0*decimals);
	    break;
	    
	  default:
	    /* cannot happen, just plugs a compiler warning */
	    fprintf (stderr, _("%s(): Unhandled fmt ` %d\n"), __FUNCTION__, fmt);
	    exit (1);
	  }
	}

	result = strtod(temp, NULL) * scale;
    }

    dprintf("    %s()=%f: fmt=%d, omit_zeros=%d, decimals=%d \n",
		    __FUNCTION__, result, fmt, omit_zeros, decimals);

    return result;
} /* read_double */