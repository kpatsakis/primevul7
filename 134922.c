PERL_STATIC_INLINE void
S_invlist_clear(pTHX_ SV* invlist)    /* Empty the inversion list */
{
    PERL_ARGS_ASSERT_INVLIST_CLEAR;

    assert(is_invlist(invlist));

    invlist_set_len(invlist, 0, 0);
    invlist_trim(invlist);