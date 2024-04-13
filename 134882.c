STATIC void
S_nextchar(pTHX_ RExC_state_t *pRExC_state)
{
    PERL_ARGS_ASSERT_NEXTCHAR;

    if (RExC_parse < RExC_end) {
        assert(   ! UTF
               || UTF8_IS_INVARIANT(*RExC_parse)
               || UTF8_IS_START(*RExC_parse));

        RExC_parse += (UTF)
                      ? UTF8_SAFE_SKIP(RExC_parse, RExC_end)
                      : 1;

        skip_to_be_ignored_text(pRExC_state, &RExC_parse,
                                FALSE /* Don't force /x */ );
    }