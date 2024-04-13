STATIC bool
S_regtail_study(pTHX_ RExC_state_t *pRExC_state, regnode_offset p,
                      const regnode_offset val, U32 depth)
{
    regnode_offset scan;
    U8 exact = PSEUDO;
#ifdef EXPERIMENTAL_INPLACESCAN
    I32 min = 0;
#endif
    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_REGTAIL_STUDY;


    /* Find last node. */

    scan = p;
    for (;;) {
        regnode * const temp = regnext(REGNODE_p(scan));
#ifdef EXPERIMENTAL_INPLACESCAN
        if (PL_regkind[OP(REGNODE_p(scan))] == EXACT) {
	    bool unfolded_multi_char;	/* Unexamined in this routine */
            if (join_exact(pRExC_state, scan, &min,
                           &unfolded_multi_char, 1, REGNODE_p(val), depth+1))
                return TRUE; /* Was return EXACT */
	}
#endif
        if ( exact ) {
            switch (OP(REGNODE_p(scan))) {
                case EXACT:
                case EXACT_ONLY8:
                case EXACTL:
                case EXACTF:
                case EXACTFU_S_EDGE:
                case EXACTFAA_NO_TRIE:
                case EXACTFAA:
                case EXACTFU:
                case EXACTFU_ONLY8:
                case EXACTFLU8:
                case EXACTFUP:
                case EXACTFL:
                        if( exact == PSEUDO )
                            exact= OP(REGNODE_p(scan));
                        else if ( exact != OP(REGNODE_p(scan)) )
                            exact= 0;
                case NOTHING:
                    break;
                default:
                    exact= 0;
            }
        }
        DEBUG_PARSE_r({
            DEBUG_PARSE_MSG((scan==p ? "tsdy" : ""));
            regprop(RExC_rx, RExC_mysv, REGNODE_p(scan), NULL, pRExC_state);
            Perl_re_printf( aTHX_  "~ %s (%d) -> %s\n",
                SvPV_nolen_const(RExC_mysv),
                scan,
                PL_reg_name[exact]);
        });
	if (temp == NULL)
	    break;
	scan = REGNODE_OFFSET(temp);
    }
    DEBUG_PARSE_r({
        DEBUG_PARSE_MSG("");
        regprop(RExC_rx, RExC_mysv, REGNODE_p(val), NULL, pRExC_state);
        Perl_re_printf( aTHX_
                      "~ attach to %s (%" IVdf ") offset to %" IVdf "\n",
		      SvPV_nolen_const(RExC_mysv),
		      (IV)val,
		      (IV)(val - scan)
        );
    });
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

    return TRUE; /* Was 'return exact' */