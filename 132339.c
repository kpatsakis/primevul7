form_fputs_decode(Str s, FILE * f)
{
    char *p;
    Str z = Strnew();

    for (p = s->ptr; *p;) {
	switch (*p) {
#if !defined( __CYGWIN__ ) && !defined( __EMX__ )
	case '\r':
	    if (*(p + 1) == '\n')
		p++;
	    /* continue to the next label */
#endif				/* !defined( __CYGWIN__ ) && !defined( __EMX__ 
				 * ) */
	default:
	    Strcat_char(z, *p);
	    p++;
	    break;
	}
    }
#ifdef USE_M17N
    z = wc_Str_conv_strict(z, InnerCharset, DisplayCharset);
#endif
    Strfputs(z, f);
}