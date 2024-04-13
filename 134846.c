STATIC bool
S_regtail(pTHX_ RExC_state_t * pRExC_state,
                const regnode_offset p,
                const regnode_offset val,
                const U32 depth)
{
    regnode_offset scan;
    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_REGTAIL;
#ifndef DEBUGGING
    PERL_UNUSED_ARG(depth);
#endif

    /* Find last node. */
    scan = (regnode_offset) p;
    for (;;) {
	regnode * const temp = regnext(REGNODE_p(scan));
        DEBUG_PARSE_r({
            DEBUG_PARSE_MSG((scan==p ? "tail" : ""));
            regprop(RExC_rx, RExC_mysv, REGNODE_p(scan), NULL, pRExC_state);
            Perl_re_printf( aTHX_  "~ %s (%d) %s %s\n",
                SvPV_nolen_const(RExC_mysv), scan,
                    (temp == NULL ? "->" : ""),
                    (temp == NULL ? PL_reg_name[OP(REGNODE_p(val))] : "")
            );
        });
        if (temp == NULL)
            break;
        scan = REGNODE_OFFSET(temp);
    }

    if (reg_off_by_arg[OP(REGNODE_p(scan))]) {
        assert((UV) (val - scan) <= U32_MAX);
        ARG_SET(REGNODE_p(scan), val - scan);
    }
    else {
        if (val - scan > U16_MAX) {
            /* Populate this with something that won't loop and will likely
             * lead to a crash if the caller ignores the failure return, and
             * execution continues */
            NEXT_OFF(REGNODE_p(scan)) = U16_MAX;
            return FALSE;
        }
        NEXT_OFF(REGNODE_p(scan)) = val - scan;
    }

    return TRUE;