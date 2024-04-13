void
Perl_re_dup_guts(pTHX_ const REGEXP *sstr, REGEXP *dstr, CLONE_PARAMS *param)
{
    dVAR;
    I32 npar;
    const struct regexp *r = ReANY(sstr);
    struct regexp *ret = ReANY(dstr);

    PERL_ARGS_ASSERT_RE_DUP_GUTS;

    npar = r->nparens+1;
    Newx(ret->offs, npar, regexp_paren_pair);
    Copy(r->offs, ret->offs, npar, regexp_paren_pair);

    if (ret->substrs) {
	/* Do it this way to avoid reading from *r after the StructCopy().
	   That way, if any of the sv_dup_inc()s dislodge *r from the L1
	   cache, it doesn't matter.  */
        int i;
	const bool anchored = r->check_substr
	    ? r->check_substr == r->substrs->data[0].substr
	    : r->check_utf8   == r->substrs->data[0].utf8_substr;
        Newx(ret->substrs, 1, struct reg_substr_data);
	StructCopy(r->substrs, ret->substrs, struct reg_substr_data);

        for (i = 0; i < 2; i++) {
            ret->substrs->data[i].substr =
                        sv_dup_inc(ret->substrs->data[i].substr, param);
            ret->substrs->data[i].utf8_substr =
                        sv_dup_inc(ret->substrs->data[i].utf8_substr, param);
        }

	/* check_substr and check_utf8, if non-NULL, point to either their
	   anchored or float namesakes, and don't hold a second reference.  */

	if (ret->check_substr) {
	    if (anchored) {
		assert(r->check_utf8 == r->substrs->data[0].utf8_substr);

		ret->check_substr = ret->substrs->data[0].substr;
		ret->check_utf8   = ret->substrs->data[0].utf8_substr;
	    } else {
		assert(r->check_substr == r->substrs->data[1].substr);
		assert(r->check_utf8   == r->substrs->data[1].utf8_substr);

		ret->check_substr = ret->substrs->data[1].substr;
		ret->check_utf8   = ret->substrs->data[1].utf8_substr;
	    }
	} else if (ret->check_utf8) {
	    if (anchored) {
		ret->check_utf8 = ret->substrs->data[0].utf8_substr;
	    } else {
		ret->check_utf8 = ret->substrs->data[1].utf8_substr;
	    }
	}
    }

    RXp_PAREN_NAMES(ret) = hv_dup_inc(RXp_PAREN_NAMES(ret), param);
    ret->qr_anoncv = MUTABLE_CV(sv_dup_inc((const SV *)ret->qr_anoncv, param));
    if (r->recurse_locinput)
        Newx(ret->recurse_locinput, r->nparens + 1, char *);

    if (ret->pprivate)
	RXi_SET(ret, CALLREGDUPE_PVT(dstr, param));

    if (RX_MATCH_COPIED(dstr))
	ret->subbeg  = SAVEPVN(ret->subbeg, ret->sublen);
    else
	ret->subbeg = NULL;
#ifdef PERL_ANY_COW
    ret->saved_copy = NULL;
#endif

    /* Whether mother_re be set or no, we need to copy the string.  We
       cannot refrain from copying it when the storage points directly to
       our mother regexp, because that's
	       1: a buffer in a different thread
	       2: something we no longer hold a reference on
	       so we need to copy it locally.  */
    RX_WRAPPED(dstr) = SAVEPVN(RX_WRAPPED_const(sstr), SvCUR(sstr)+1);
    /* set malloced length to a non-zero value so it will be freed
     * (otherwise in combination with SVf_FAKE it looks like an alien
     * buffer). It doesn't have to be the actual malloced size, since it
     * should never be grown */
    SvLEN_set(dstr, SvCUR(sstr)+1);
    ret->mother_re   = NULL;