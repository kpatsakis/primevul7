REGEXP *
Perl_reg_temp_copy(pTHX_ REGEXP *dsv, REGEXP *ssv)
{
    struct regexp *drx;
    struct regexp *const srx = ReANY(ssv);
    const bool islv = dsv && SvTYPE(dsv) == SVt_PVLV;

    PERL_ARGS_ASSERT_REG_TEMP_COPY;

    if (!dsv)
	dsv = (REGEXP*) newSV_type(SVt_REGEXP);
    else {
        assert(SvTYPE(dsv) == SVt_REGEXP || (SvTYPE(dsv) == SVt_PVLV));

        /* our only valid caller, sv_setsv_flags(), should have done
         * a SV_CHECK_THINKFIRST_COW_DROP() by now */
        assert(!SvOOK(dsv));
        assert(!SvIsCOW(dsv));
        assert(!SvROK(dsv));

        if (SvPVX_const(dsv)) {
            if (SvLEN(dsv))
                Safefree(SvPVX(dsv));
            SvPVX(dsv) = NULL;
        }
        SvLEN_set(dsv, 0);
        SvCUR_set(dsv, 0);
	SvOK_off((SV *)dsv);

	if (islv) {
	    /* For PVLVs, the head (sv_any) points to an XPVLV, while
             * the LV's xpvlenu_rx will point to a regexp body, which
             * we allocate here */
	    REGEXP *temp = (REGEXP *)newSV_type(SVt_REGEXP);
	    assert(!SvPVX(dsv));
            ((XPV*)SvANY(dsv))->xpv_len_u.xpvlenu_rx = temp->sv_any;
	    temp->sv_any = NULL;
	    SvFLAGS(temp) = (SvFLAGS(temp) & ~SVTYPEMASK) | SVt_NULL;
	    SvREFCNT_dec_NN(temp);
	    /* SvCUR still resides in the xpvlv struct, so the regexp copy-
	       ing below will not set it. */
	    SvCUR_set(dsv, SvCUR(ssv));
	}
    }
    /* This ensures that SvTHINKFIRST(sv) is true, and hence that
       sv_force_normal(sv) is called.  */
    SvFAKE_on(dsv);
    drx = ReANY(dsv);

    SvFLAGS(dsv) |= SvFLAGS(ssv) & (SVf_POK|SVp_POK|SVf_UTF8);
    SvPV_set(dsv, RX_WRAPPED(ssv));
    /* We share the same string buffer as the original regexp, on which we
       hold a reference count, incremented when mother_re is set below.
       The string pointer is copied here, being part of the regexp struct.
     */
    memcpy(&(drx->xpv_cur), &(srx->xpv_cur),
	   sizeof(regexp) - STRUCT_OFFSET(regexp, xpv_cur));
    if (!islv)
        SvLEN_set(dsv, 0);
    if (srx->offs) {
        const I32 npar = srx->nparens+1;
        Newx(drx->offs, npar, regexp_paren_pair);
        Copy(srx->offs, drx->offs, npar, regexp_paren_pair);
    }
    if (srx->substrs) {
        int i;
        Newx(drx->substrs, 1, struct reg_substr_data);
	StructCopy(srx->substrs, drx->substrs, struct reg_substr_data);

        for (i = 0; i < 2; i++) {
            SvREFCNT_inc_void(drx->substrs->data[i].substr);
            SvREFCNT_inc_void(drx->substrs->data[i].utf8_substr);
        }

	/* check_substr and check_utf8, if non-NULL, point to either their
	   anchored or float namesakes, and don't hold a second reference.  */
    }
    RX_MATCH_COPIED_off(dsv);
#ifdef PERL_ANY_COW
    drx->saved_copy = NULL;
#endif
    drx->mother_re = ReREFCNT_inc(srx->mother_re ? srx->mother_re : ssv);
    SvREFCNT_inc_void(drx->qr_anoncv);
    if (srx->recurse_locinput)
        Newx(drx->recurse_locinput, srx->nparens + 1, char *);

    return dsv;