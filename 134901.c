PERL_STATIC_INLINE char*
S_reg_skipcomment(RExC_state_t *pRExC_state, char* p)
{
    PERL_ARGS_ASSERT_REG_SKIPCOMMENT;

    assert(*p == '#');

    while (p < RExC_end) {
        if (*(++p) == '\n') {
            return p+1;
        }
    }

    /* we ran off the end of the pattern without ending the comment, so we have
     * to add an \n when wrapping */
    RExC_seen |= REG_RUN_ON_COMMENT_SEEN;
    return p;