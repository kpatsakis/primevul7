static void
S_regdump_extflags(pTHX_ const char *lead, const U32 flags)
{
    int bit;
    int set=0;
    regex_charset cs;

    ASSUME(REG_EXTFLAGS_NAME_SIZE <= sizeof(flags)*8);

    for (bit=0; bit<REG_EXTFLAGS_NAME_SIZE; bit++) {
        if (flags & (1<<bit)) {
	    if ((1<<bit) & RXf_PMf_CHARSET) {	/* Output separately, below */
		continue;
	    }
            if (!set++ && lead)
                Perl_re_printf( aTHX_  "%s", lead);
            Perl_re_printf( aTHX_  "%s ", PL_reg_extflags_name[bit]);
        }
    }
    if ((cs = get_regex_charset(flags)) != REGEX_DEPENDS_CHARSET) {
            if (!set++ && lead) {
                Perl_re_printf( aTHX_  "%s", lead);
            }
            switch (cs) {
                case REGEX_UNICODE_CHARSET:
                    Perl_re_printf( aTHX_  "UNICODE");
                    break;
                case REGEX_LOCALE_CHARSET:
                    Perl_re_printf( aTHX_  "LOCALE");
                    break;
                case REGEX_ASCII_RESTRICTED_CHARSET:
                    Perl_re_printf( aTHX_  "ASCII-RESTRICTED");
                    break;
                case REGEX_ASCII_MORE_RESTRICTED_CHARSET:
                    Perl_re_printf( aTHX_  "ASCII-MORE_RESTRICTED");
                    break;
                default:
                    Perl_re_printf( aTHX_  "UNKNOWN CHARACTER SET");
                    break;
            }
    }
    if (lead)  {
        if (set)
            Perl_re_printf( aTHX_  "\n");
        else
            Perl_re_printf( aTHX_  "%s[none-set]\n", lead);
    }