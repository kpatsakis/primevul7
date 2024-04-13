STATIC const regnode *
S_dumpuntil(pTHX_ const regexp *r, const regnode *start, const regnode *node,
	    const regnode *last, const regnode *plast,
	    SV* sv, I32 indent, U32 depth)
{
    U8 op = PSEUDO;	/* Arbitrary non-END op. */
    const regnode *next;
    const regnode *optstart= NULL;

    RXi_GET_DECL(r, ri);
    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_DUMPUNTIL;

#ifdef DEBUG_DUMPUNTIL
    Perl_re_printf( aTHX_  "--- %d : %d - %d - %d\n", indent, node-start,
        last ? last-start : 0, plast ? plast-start : 0);
#endif

    if (plast && plast < last)
        last= plast;

    while (PL_regkind[op] != END && (!last || node < last)) {
        assert(node);
	/* While that wasn't END last time... */
	NODE_ALIGN(node);
	op = OP(node);
	if (op == CLOSE || op == SRCLOSE || op == WHILEM)
	    indent--;
	next = regnext((regnode *)node);

	/* Where, what. */
	if (OP(node) == OPTIMIZED) {
	    if (!optstart && RE_DEBUG_FLAG(RE_DEBUG_COMPILE_OPTIMISE))
	        optstart = node;
	    else
		goto after_print;
	} else
	    CLEAR_OPTSTART;

        regprop(r, sv, node, NULL, NULL);
        Perl_re_printf( aTHX_  "%4" IVdf ":%*s%s", (IV)(node - start),
		      (int)(2*indent + 1), "", SvPVX_const(sv));

        if (OP(node) != OPTIMIZED) {
            if (next == NULL)		/* Next ptr. */
                Perl_re_printf( aTHX_  " (0)");
            else if (PL_regkind[(U8)op] == BRANCH
                     && PL_regkind[OP(next)] != BRANCH )
                Perl_re_printf( aTHX_  " (FAIL)");
            else
                Perl_re_printf( aTHX_  " (%" IVdf ")", (IV)(next - start));
            Perl_re_printf( aTHX_ "\n");
        }

      after_print:
	if (PL_regkind[(U8)op] == BRANCHJ) {
	    assert(next);
	    {
                const regnode *nnode = (OP(next) == LONGJMP
                                       ? regnext((regnode *)next)
                                       : next);
                if (last && nnode > last)
                    nnode = last;
                DUMPUNTIL(NEXTOPER(NEXTOPER(node)), nnode);
	    }
	}
	else if (PL_regkind[(U8)op] == BRANCH) {
	    assert(next);
	    DUMPUNTIL(NEXTOPER(node), next);
	}
	else if ( PL_regkind[(U8)op]  == TRIE ) {
	    const regnode *this_trie = node;
	    const char op = OP(node);
            const U32 n = ARG(node);
	    const reg_ac_data * const ac = op>=AHOCORASICK ?
               (reg_ac_data *)ri->data->data[n] :
               NULL;
	    const reg_trie_data * const trie =
	        (reg_trie_data*)ri->data->data[op<AHOCORASICK ? n : ac->trie];
#ifdef DEBUGGING
	    AV *const trie_words
                           = MUTABLE_AV(ri->data->data[n + TRIE_WORDS_OFFSET]);
#endif
	    const regnode *nextbranch= NULL;
	    I32 word_idx;
            SvPVCLEAR(sv);
	    for (word_idx= 0; word_idx < (I32)trie->wordcount; word_idx++) {
		SV ** const elem_ptr = av_fetch(trie_words, word_idx, 0);

                Perl_re_indentf( aTHX_  "%s ",
                    indent+3,
                    elem_ptr
                    ? pv_pretty(sv, SvPV_nolen_const(*elem_ptr),
                                SvCUR(*elem_ptr), PL_dump_re_max_len,
                                PL_colors[0], PL_colors[1],
                                (SvUTF8(*elem_ptr)
                                 ? PERL_PV_ESCAPE_UNI
                                 : 0)
                                | PERL_PV_PRETTY_ELLIPSES
                                | PERL_PV_PRETTY_LTGT
                            )
                    : "???"
                );
                if (trie->jump) {
                    U16 dist= trie->jump[word_idx+1];
                    Perl_re_printf( aTHX_  "(%" UVuf ")\n",
                               (UV)((dist ? this_trie + dist : next) - start));
                    if (dist) {
                        if (!nextbranch)
                            nextbranch= this_trie + trie->jump[0];
			DUMPUNTIL(this_trie + dist, nextbranch);
                    }
                    if (nextbranch && PL_regkind[OP(nextbranch)]==BRANCH)
                        nextbranch= regnext((regnode *)nextbranch);
                } else {
                    Perl_re_printf( aTHX_  "\n");
		}
	    }
	    if (last && next > last)
	        node= last;
	    else
	        node= next;
	}
	else if ( op == CURLY ) {   /* "next" might be very big: optimizer */
	    DUMPUNTIL(NEXTOPER(node) + EXTRA_STEP_2ARGS,
                    NEXTOPER(node) + EXTRA_STEP_2ARGS + 1);
	}
	else if (PL_regkind[(U8)op] == CURLY && op != CURLYX) {
	    assert(next);
	    DUMPUNTIL(NEXTOPER(node) + EXTRA_STEP_2ARGS, next);
	}
	else if ( op == PLUS || op == STAR) {
	    DUMPUNTIL(NEXTOPER(node), NEXTOPER(node) + 1);
	}
	else if (PL_regkind[(U8)op] == EXACT) {
            /* Literal string, where present. */
	    node += NODE_SZ_STR(node) - 1;
	    node = NEXTOPER(node);
	}
	else {
	    node = NEXTOPER(node);
	    node += regarglen[(U8)op];
	}
	if (op == CURLYX || op == OPEN || op == SROPEN)
	    indent++;
    }
    CLEAR_OPTSTART;
#ifdef DEBUG_DUMPUNTIL
    Perl_re_printf( aTHX_  "--- %d\n", (int)indent);
#endif
    return node;