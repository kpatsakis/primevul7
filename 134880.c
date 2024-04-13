void
Perl_pregfree2(pTHX_ REGEXP *rx)
{
    struct regexp *const r = ReANY(rx);
    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_PREGFREE2;

    if (! r)
        return;

    if (r->mother_re) {
        ReREFCNT_dec(r->mother_re);
    } else {
        CALLREGFREE_PVT(rx); /* free the private data */
        SvREFCNT_dec(RXp_PAREN_NAMES(r));
    }
    if (r->substrs) {
        int i;
        for (i = 0; i < 2; i++) {
            SvREFCNT_dec(r->substrs->data[i].substr);
            SvREFCNT_dec(r->substrs->data[i].utf8_substr);
        }
	Safefree(r->substrs);
    }
    RX_MATCH_COPY_FREE(rx);
#ifdef PERL_ANY_COW
    SvREFCNT_dec(r->saved_copy);
#endif
    Safefree(r->offs);
    SvREFCNT_dec(r->qr_anoncv);
    if (r->recurse_locinput)
        Safefree(r->recurse_locinput);