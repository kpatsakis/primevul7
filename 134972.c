STATIC regnode_offset
S_regnode_guts(pTHX_ RExC_state_t *pRExC_state, const U8 op, const STRLEN extra_size, const char* const name)
{
    /* Allocate a regnode for 'op', with 'extra_size' extra space.  It aligns
     * and increments RExC_size and RExC_emit
     *
     * It returns the regnode's offset into the regex engine program */

    const regnode_offset ret = RExC_emit;

    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_REGNODE_GUTS;

    SIZE_ALIGN(RExC_size);
    change_engine_size(pRExC_state, (Ptrdiff_t) 1 + extra_size);
    NODE_ALIGN_FILL(REGNODE_p(ret));
#ifndef RE_TRACK_PATTERN_OFFSETS
    PERL_UNUSED_ARG(name);
    PERL_UNUSED_ARG(op);
#else
    assert(extra_size >= regarglen[op] || PL_regkind[op] == ANYOF);

    if (RExC_offsets) {         /* MJD */
	MJD_OFFSET_DEBUG(
              ("%s:%d: (op %s) %s %" UVuf " (len %" UVuf ") (max %" UVuf ").\n",
              name, __LINE__,
              PL_reg_name[op],
              (UV)(RExC_emit) > RExC_offsets[0]
		? "Overwriting end of array!\n" : "OK",
              (UV)(RExC_emit),
              (UV)(RExC_parse - RExC_start),
              (UV)RExC_offsets[0]));
	Set_Node_Offset(REGNODE_p(RExC_emit), RExC_parse + (op == END));
    }
#endif
    return(ret);