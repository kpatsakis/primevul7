static void
S_regdump_intflags(pTHX_ const char *lead, const U32 flags)
{
    int bit;
    int set=0;

    ASSUME(REG_INTFLAGS_NAME_SIZE <= sizeof(flags)*8);

    for (bit=0; bit<REG_INTFLAGS_NAME_SIZE; bit++) {
        if (flags & (1<<bit)) {
            if (!set++ && lead)
                Perl_re_printf( aTHX_  "%s", lead);
            Perl_re_printf( aTHX_  "%s ", PL_reg_intflags_name[bit]);
        }
    }
    if (lead)  {
        if (set)
            Perl_re_printf( aTHX_  "\n");
        else
            Perl_re_printf( aTHX_  "%s[none-set]\n", lead);
    }