STATIC void
S_change_engine_size(pTHX_ RExC_state_t *pRExC_state, const Ptrdiff_t size)
{
    /* 'size' is the delta to add or subtract from the current memory allocated
     * to the regex engine being constructed */

    PERL_ARGS_ASSERT_CHANGE_ENGINE_SIZE;

    RExC_size += size;

    Renewc(RExC_rxi,
           sizeof(regexp_internal) + (RExC_size + 1) * sizeof(regnode),
                                                /* +1 for REG_MAGIC */
           char,
           regexp_internal);
    if ( RExC_rxi == NULL )
	FAIL("Regexp out of space");
    RXi_SET(RExC_rx, RExC_rxi);

    RExC_emit_start = RExC_rxi->program;
    if (size > 0) {
        Zero(REGNODE_p(RExC_emit), size, regnode);
    }

#ifdef RE_TRACK_PATTERN_OFFSETS
    Renew(RExC_offsets, 2*RExC_size+1, U32);
    if (size > 0) {
        Zero(RExC_offsets + 2*(RExC_size - size) + 1, 2 * size, U32);
    }
    RExC_offsets[0] = RExC_size;
#endif