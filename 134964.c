STATIC void
S_populate_ANYOF_from_invlist(pTHX_ regnode *node, SV** invlist_ptr)
{
    /* Uses the inversion list '*invlist_ptr' to populate the ANYOF 'node'.  It
     * sets up the bitmap and any flags, removing those code points from the
     * inversion list, setting it to NULL should it become completely empty */

    dVAR;

    PERL_ARGS_ASSERT_POPULATE_ANYOF_FROM_INVLIST;
    assert(PL_regkind[OP(node)] == ANYOF);

    /* There is no bitmap for this node type */
    if (OP(node) == ANYOFH) {
        return;
    }

    ANYOF_BITMAP_ZERO(node);
    if (*invlist_ptr) {

	/* This gets set if we actually need to modify things */
	bool change_invlist = FALSE;

	UV start, end;

	/* Start looking through *invlist_ptr */
	invlist_iterinit(*invlist_ptr);
	while (invlist_iternext(*invlist_ptr, &start, &end)) {
	    UV high;
	    int i;

            if (end == UV_MAX && start <= NUM_ANYOF_CODE_POINTS) {
                ANYOF_FLAGS(node) |= ANYOF_MATCHES_ALL_ABOVE_BITMAP;
            }

	    /* Quit if are above what we should change */
	    if (start >= NUM_ANYOF_CODE_POINTS) {
		break;
	    }

	    change_invlist = TRUE;

	    /* Set all the bits in the range, up to the max that we are doing */
	    high = (end < NUM_ANYOF_CODE_POINTS - 1)
                   ? end
                   : NUM_ANYOF_CODE_POINTS - 1;
	    for (i = start; i <= (int) high; i++) {
		if (! ANYOF_BITMAP_TEST(node, i)) {
		    ANYOF_BITMAP_SET(node, i);
		}
	    }
	}
	invlist_iterfinish(*invlist_ptr);

        /* Done with loop; remove any code points that are in the bitmap from
         * *invlist_ptr; similarly for code points above the bitmap if we have
         * a flag to match all of them anyways */
	if (change_invlist) {
	    _invlist_subtract(*invlist_ptr, PL_InBitmap, invlist_ptr);
	}
        if (ANYOF_FLAGS(node) & ANYOF_MATCHES_ALL_ABOVE_BITMAP) {
	    _invlist_intersection(*invlist_ptr, PL_InBitmap, invlist_ptr);
	}

	/* If have completely emptied it, remove it completely */
	if (_invlist_len(*invlist_ptr) == 0) {
	    SvREFCNT_dec_NN(*invlist_ptr);
	    *invlist_ptr = NULL;
	}
    }