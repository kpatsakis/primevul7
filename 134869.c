STATIC void
S_set_regex_pv(pTHX_ RExC_state_t *pRExC_state, REGEXP *Rx)
{
    /* Calculates and sets in the compiled pattern 'Rx' the string to compile,
     * properly wrapped with the right modifiers */

    bool has_p     = ((RExC_rx->extflags & RXf_PMf_KEEPCOPY) == RXf_PMf_KEEPCOPY);
    bool has_charset = RExC_utf8 || (get_regex_charset(RExC_rx->extflags)
                                                != REGEX_DEPENDS_CHARSET);

    /* The caret is output if there are any defaults: if not all the STD
        * flags are set, or if no character set specifier is needed */
    bool has_default =
                (((RExC_rx->extflags & RXf_PMf_STD_PMMOD) != RXf_PMf_STD_PMMOD)
                || ! has_charset);
    bool has_runon = ((RExC_seen & REG_RUN_ON_COMMENT_SEEN)
                                                == REG_RUN_ON_COMMENT_SEEN);
    U8 reganch = (U8)((RExC_rx->extflags & RXf_PMf_STD_PMMOD)
                        >> RXf_PMf_STD_PMMOD_SHIFT);
    const char *fptr = STD_PAT_MODS;        /*"msixxn"*/
    char *p;
    STRLEN pat_len = RExC_precomp_end - RExC_precomp;

    /* We output all the necessary flags; we never output a minus, as all
        * those are defaults, so are
        * covered by the caret */
    const STRLEN wraplen = pat_len + has_p + has_runon
        + has_default       /* If needs a caret */
        + PL_bitcount[reganch] /* 1 char for each set standard flag */

            /* If needs a character set specifier */
        + ((has_charset) ? MAX_CHARSET_NAME_LENGTH : 0)
        + (sizeof("(?:)") - 1);

    PERL_ARGS_ASSERT_SET_REGEX_PV;

    /* make sure PL_bitcount bounds not exceeded */
    assert(sizeof(STD_PAT_MODS) <= 8);

    p = sv_grow(MUTABLE_SV(Rx), wraplen + 1); /* +1 for the ending NUL */
    SvPOK_on(Rx);
    if (RExC_utf8)
        SvFLAGS(Rx) |= SVf_UTF8;
    *p++='('; *p++='?';

    /* If a default, cover it using the caret */
    if (has_default) {
        *p++= DEFAULT_PAT_MOD;
    }
    if (has_charset) {
        STRLEN len;
        const char* name;

        name = get_regex_charset_name(RExC_rx->extflags, &len);
        if strEQ(name, DEPENDS_PAT_MODS) {  /* /d under UTF-8 => /u */
            assert(RExC_utf8);
            name = UNICODE_PAT_MODS;
            len = sizeof(UNICODE_PAT_MODS) - 1;
        }
        Copy(name, p, len, char);
        p += len;
    }
    if (has_p)
        *p++ = KEEPCOPY_PAT_MOD; /*'p'*/
    {
        char ch;
        while((ch = *fptr++)) {
            if(reganch & 1)
                *p++ = ch;
            reganch >>= 1;
        }
    }

    *p++ = ':';
    Copy(RExC_precomp, p, pat_len, char);
    assert ((RX_WRAPPED(Rx) - p) < 16);
    RExC_rx->pre_prefix = p - RX_WRAPPED(Rx);
    p += pat_len;

    /* Adding a trailing \n causes this to compile properly:
            my $R = qr / A B C # D E/x; /($R)/
        Otherwise the parens are considered part of the comment */
    if (has_runon)
        *p++ = '\n';
    *p++ = ')';
    *p = 0;
    SvCUR_set(Rx, p - RX_WRAPPED(Rx));