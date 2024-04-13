STATIC void
S_reginsert(pTHX_ RExC_state_t *pRExC_state, const U8 op,
                  const regnode_offset operand, const U32 depth)
{
    regnode *src;
    regnode *dst;
    regnode *place;
    const int offset = regarglen[(U8)op];
    const int size = NODE_STEP_REGNODE + offset;
    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_REGINSERT;
    PERL_UNUSED_CONTEXT;
    PERL_UNUSED_ARG(depth);
/* (PL_regkind[(U8)op] == CURLY ? EXTRA_STEP_2ARGS : 0); */
    DEBUG_PARSE_FMT("inst"," - %s", PL_reg_name[op]);
    assert(!RExC_study_started); /* I believe we should never use reginsert once we have started
                                    studying. If this is wrong then we need to adjust RExC_recurse
                                    below like we do with RExC_open_parens/RExC_close_parens. */
    change_engine_size(pRExC_state, (Ptrdiff_t) size);
    src = REGNODE_p(RExC_emit);
    RExC_emit += size;
    dst = REGNODE_p(RExC_emit);

    /* If we are in a "count the parentheses" pass, the numbers are unreliable,
     * and [perl #133871] shows this can lead to problems, so skip this
     * realignment of parens until a later pass when they are reliable */
    if (! IN_PARENS_PASS && RExC_open_parens) {
        int paren;
        /*DEBUG_PARSE_FMT("inst"," - %" IVdf, (IV)RExC_npar);*/
        /* remember that RExC_npar is rex->nparens + 1,
         * iow it is 1 more than the number of parens seen in
         * the pattern so far. */
        for ( paren=0 ; paren < RExC_npar ; paren++ ) {
            /* note, RExC_open_parens[0] is the start of the
             * regex, it can't move. RExC_close_parens[0] is the end
             * of the regex, it *can* move. */
            if ( paren && RExC_open_parens[paren] >= operand ) {
                /*DEBUG_PARSE_FMT("open"," - %d", size);*/
                RExC_open_parens[paren] += size;
            } else {
                /*DEBUG_PARSE_FMT("open"," - %s","ok");*/
            }
            if ( RExC_close_parens[paren] >= operand ) {
                /*DEBUG_PARSE_FMT("close"," - %d", size);*/
                RExC_close_parens[paren] += size;
            } else {
                /*DEBUG_PARSE_FMT("close"," - %s","ok");*/
            }
        }
    }
    if (RExC_end_op)
        RExC_end_op += size;

    while (src > REGNODE_p(operand)) {
	StructCopy(--src, --dst, regnode);
#ifdef RE_TRACK_PATTERN_OFFSETS
        if (RExC_offsets) {     /* MJD 20010112 */
	    MJD_OFFSET_DEBUG(
                 ("%s(%d): (op %s) %s copy %" UVuf " -> %" UVuf " (max %" UVuf ").\n",
                  "reginsert",
		  __LINE__,
		  PL_reg_name[op],
                  (UV)(REGNODE_OFFSET(dst)) > RExC_offsets[0]
		    ? "Overwriting end of array!\n" : "OK",
                  (UV)REGNODE_OFFSET(src),
                  (UV)REGNODE_OFFSET(dst),
                  (UV)RExC_offsets[0]));
	    Set_Node_Offset_To_R(REGNODE_OFFSET(dst), Node_Offset(src));
	    Set_Node_Length_To_R(REGNODE_OFFSET(dst), Node_Length(src));
        }
#endif
    }

    place = REGNODE_p(operand);	/* Op node, where operand used to be. */
#ifdef RE_TRACK_PATTERN_OFFSETS
    if (RExC_offsets) {         /* MJD */
	MJD_OFFSET_DEBUG(
              ("%s(%d): (op %s) %s %" UVuf " <- %" UVuf " (max %" UVuf ").\n",
              "reginsert",
	      __LINE__,
	      PL_reg_name[op],
              (UV)REGNODE_OFFSET(place) > RExC_offsets[0]
              ? "Overwriting end of array!\n" : "OK",
              (UV)REGNODE_OFFSET(place),
              (UV)(RExC_parse - RExC_start),
              (UV)RExC_offsets[0]));
	Set_Node_Offset(place, RExC_parse);
	Set_Node_Length(place, 1);
    }
#endif
    src = NEXTOPER(place);
    FLAGS(place) = 0;
    FILL_NODE(operand, op);

    /* Zero out any arguments in the new node */
    Zero(src, offset, regnode);