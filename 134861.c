STATIC regnode_offset
S_regbranch(pTHX_ RExC_state_t *pRExC_state, I32 *flagp, I32 first, U32 depth)
{
    regnode_offset ret;
    regnode_offset chain = 0;
    regnode_offset latest;
    I32 flags = 0, c = 0;
    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_REGBRANCH;

    DEBUG_PARSE("brnc");

    if (first)
	ret = 0;
    else {
	if (RExC_use_BRANCHJ)
	    ret = reganode(pRExC_state, BRANCHJ, 0);
	else {
	    ret = reg_node(pRExC_state, BRANCH);
            Set_Node_Length(REGNODE_p(ret), 1);
        }
    }

    *flagp = WORST;			/* Tentatively. */

    skip_to_be_ignored_text(pRExC_state, &RExC_parse,
                            FALSE /* Don't force to /x */ );
    while (RExC_parse < RExC_end && *RExC_parse != '|' && *RExC_parse != ')') {
	flags &= ~TRYAGAIN;
        latest = regpiece(pRExC_state, &flags, depth+1);
	if (latest == 0) {
	    if (flags & TRYAGAIN)
		continue;
            RETURN_FAIL_ON_RESTART(flags, flagp);
            FAIL2("panic: regpiece returned failure, flags=%#" UVxf, (UV) flags);
	}
	else if (ret == 0)
            ret = latest;
	*flagp |= flags&(HASWIDTH|POSTPONED);
	if (chain == 0) 	/* First piece. */
	    *flagp |= flags&SPSTART;
	else {
	    /* FIXME adding one for every branch after the first is probably
	     * excessive now we have TRIE support. (hv) */
	    MARK_NAUGHTY(1);
            if (! REGTAIL(pRExC_state, chain, latest)) {
                /* XXX We could just redo this branch, but figuring out what
                 * bookkeeping needs to be reset is a pain, and it's likely
                 * that other branches that goto END will also be too large */
                REQUIRE_BRANCHJ(flagp, 0);
            }
	}
	chain = latest;
	c++;
    }
    if (chain == 0) {	/* Loop ran zero times. */
	chain = reg_node(pRExC_state, NOTHING);
	if (ret == 0)
	    ret = chain;
    }
    if (c == 1) {
	*flagp |= flags&SIMPLE;
    }

    return ret;