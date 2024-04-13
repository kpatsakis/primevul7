STATIC regnode_offset
S_regpiece(pTHX_ RExC_state_t *pRExC_state, I32 *flagp, U32 depth)
{
    regnode_offset ret;
    char op;
    char *next;
    I32 flags;
    const char * const origparse = RExC_parse;
    I32 min;
    I32 max = REG_INFTY;
#ifdef RE_TRACK_PATTERN_OFFSETS
    char *parse_start;
#endif
    const char *maxpos = NULL;
    UV uv;

    /* Save the original in case we change the emitted regop to a FAIL. */
    const regnode_offset orig_emit = RExC_emit;

    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_REGPIECE;

    DEBUG_PARSE("piec");

    ret = regatom(pRExC_state, &flags, depth+1);
    if (ret == 0) {
        RETURN_FAIL_ON_RESTART_OR_FLAGS(flags, flagp, TRYAGAIN);
        FAIL2("panic: regatom returned failure, flags=%#" UVxf, (UV) flags);
    }

    op = *RExC_parse;

    if (op == '{' && regcurly(RExC_parse)) {
	maxpos = NULL;
#ifdef RE_TRACK_PATTERN_OFFSETS
        parse_start = RExC_parse; /* MJD */
#endif
	next = RExC_parse + 1;
	while (isDIGIT(*next) || *next == ',') {
	    if (*next == ',') {
		if (maxpos)
		    break;
		else
		    maxpos = next;
	    }
	    next++;
	}
	if (*next == '}') {		/* got one */
            const char* endptr;
	    if (!maxpos)
		maxpos = next;
	    RExC_parse++;
            if (isDIGIT(*RExC_parse)) {
                endptr = RExC_end;
                if (!grok_atoUV(RExC_parse, &uv, &endptr))
                    vFAIL("Invalid quantifier in {,}");
                if (uv >= REG_INFTY)
                    vFAIL2("Quantifier in {,} bigger than %d", REG_INFTY - 1);
                min = (I32)uv;
            } else {
                min = 0;
            }
	    if (*maxpos == ',')
		maxpos++;
	    else
		maxpos = RExC_parse;
            if (isDIGIT(*maxpos)) {
                endptr = RExC_end;
                if (!grok_atoUV(maxpos, &uv, &endptr))
                    vFAIL("Invalid quantifier in {,}");
                if (uv >= REG_INFTY)
                    vFAIL2("Quantifier in {,} bigger than %d", REG_INFTY - 1);
                max = (I32)uv;
            } else {
		max = REG_INFTY;		/* meaning "infinity" */
            }
	    RExC_parse = next;
	    nextchar(pRExC_state);
            if (max < min) {    /* If can't match, warn and optimize to fail
                                   unconditionally */
                reginsert(pRExC_state, OPFAIL, orig_emit, depth+1);
                ckWARNreg(RExC_parse, "Quantifier {n,m} with n > m can't match");
                NEXT_OFF(REGNODE_p(orig_emit)) =
                                    regarglen[OPFAIL] + NODE_STEP_REGNODE;
                return ret;
            }
            else if (min == max && *RExC_parse == '?')
            {
                ckWARN2reg(RExC_parse + 1,
                           "Useless use of greediness modifier '%c'",
                           *RExC_parse);
            }

	  do_curly:
	    if ((flags&SIMPLE)) {
                if (min == 0 && max == REG_INFTY) {
                    reginsert(pRExC_state, STAR, ret, depth+1);
                    MARK_NAUGHTY(4);
                    RExC_seen |= REG_UNBOUNDED_QUANTIFIER_SEEN;
                    goto nest_check;
                }
                if (min == 1 && max == REG_INFTY) {
                    reginsert(pRExC_state, PLUS, ret, depth+1);
                    MARK_NAUGHTY(3);
                    RExC_seen |= REG_UNBOUNDED_QUANTIFIER_SEEN;
                    goto nest_check;
                }
                MARK_NAUGHTY_EXP(2, 2);
		reginsert(pRExC_state, CURLY, ret, depth+1);
                Set_Node_Offset(REGNODE_p(ret), parse_start+1); /* MJD */
                Set_Node_Cur_Length(REGNODE_p(ret), parse_start);
	    }
	    else {
		const regnode_offset w = reg_node(pRExC_state, WHILEM);

		FLAGS(REGNODE_p(w)) = 0;
                if (!  REGTAIL(pRExC_state, ret, w)) {
                    REQUIRE_BRANCHJ(flagp, 0);
                }
		if (RExC_use_BRANCHJ) {
		    reginsert(pRExC_state, LONGJMP, ret, depth+1);
		    reginsert(pRExC_state, NOTHING, ret, depth+1);
		    NEXT_OFF(REGNODE_p(ret)) = 3;	/* Go over LONGJMP. */
		}
		reginsert(pRExC_state, CURLYX, ret, depth+1);
                                /* MJD hk */
                Set_Node_Offset(REGNODE_p(ret), parse_start+1);
                Set_Node_Length(REGNODE_p(ret),
                                op == '{' ? (RExC_parse - parse_start) : 1);

		if (RExC_use_BRANCHJ)
                    NEXT_OFF(REGNODE_p(ret)) = 3;   /* Go over NOTHING to
                                                       LONGJMP. */
                if (! REGTAIL(pRExC_state, ret, reg_node(pRExC_state,
                                                          NOTHING)))
                {
                    REQUIRE_BRANCHJ(flagp, 0);
                }
                RExC_whilem_seen++;
                MARK_NAUGHTY_EXP(1, 4);     /* compound interest */
	    }
	    FLAGS(REGNODE_p(ret)) = 0;

	    if (min > 0)
		*flagp = WORST;
	    if (max > 0)
		*flagp |= HASWIDTH;
            ARG1_SET(REGNODE_p(ret), (U16)min);
            ARG2_SET(REGNODE_p(ret), (U16)max);
            if (max == REG_INFTY)
                RExC_seen |= REG_UNBOUNDED_QUANTIFIER_SEEN;

	    goto nest_check;
	}
    }

    if (!ISMULT1(op)) {
	*flagp = flags;
	return(ret);
    }

#if 0				/* Now runtime fix should be reliable. */

    /* if this is reinstated, don't forget to put this back into perldiag:

	    =item Regexp *+ operand could be empty at {#} in regex m/%s/

	   (F) The part of the regexp subject to either the * or + quantifier
           could match an empty string. The {#} shows in the regular
           expression about where the problem was discovered.

    */

    if (!(flags&HASWIDTH) && op != '?')
      vFAIL("Regexp *+ operand could be empty");
#endif

#ifdef RE_TRACK_PATTERN_OFFSETS
    parse_start = RExC_parse;
#endif
    nextchar(pRExC_state);

    *flagp = (op != '+') ? (WORST|SPSTART|HASWIDTH) : (WORST|HASWIDTH);

    if (op == '*') {
	min = 0;
	goto do_curly;
    }
    else if (op == '+') {
	min = 1;
	goto do_curly;
    }
    else if (op == '?') {
	min = 0; max = 1;
	goto do_curly;
    }
  nest_check:
    if (!(flags&(HASWIDTH|POSTPONED)) && max > REG_INFTY/3) {
	ckWARN2reg(RExC_parse,
		   "%" UTF8f " matches null string many times",
		   UTF8fARG(UTF, (RExC_parse >= origparse
                                 ? RExC_parse - origparse
                                 : 0),
		   origparse));
    }

    if (*RExC_parse == '?') {
	nextchar(pRExC_state);
	reginsert(pRExC_state, MINMOD, ret, depth+1);
        if (! REGTAIL(pRExC_state, ret, ret + NODE_STEP_REGNODE)) {
            REQUIRE_BRANCHJ(flagp, 0);
        }
    }
    else if (*RExC_parse == '+') {
        regnode_offset ender;
        nextchar(pRExC_state);
        ender = reg_node(pRExC_state, SUCCEED);
        if (! REGTAIL(pRExC_state, ret, ender)) {
            REQUIRE_BRANCHJ(flagp, 0);
        }
        reginsert(pRExC_state, SUSPEND, ret, depth+1);
        ender = reg_node(pRExC_state, TAIL);
        if (! REGTAIL(pRExC_state, ret, ender)) {
            REQUIRE_BRANCHJ(flagp, 0);
        }
    }

    if (ISMULT2(RExC_parse)) {
	RExC_parse++;
	vFAIL("Nested quantifiers");
    }

    return(ret);