void
Perl_regprop(pTHX_ const regexp *prog, SV *sv, const regnode *o, const regmatch_info *reginfo, const RExC_state_t *pRExC_state)
{
#ifdef DEBUGGING
    dVAR;
    int k;
    RXi_GET_DECL(prog, progi);
    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_REGPROP;

    SvPVCLEAR(sv);

    if (OP(o) > REGNODE_MAX)		/* regnode.type is unsigned */
	/* It would be nice to FAIL() here, but this may be called from
	   regexec.c, and it would be hard to supply pRExC_state. */
	Perl_croak(aTHX_ "Corrupted regexp opcode %d > %d",
                                              (int)OP(o), (int)REGNODE_MAX);
    sv_catpv(sv, PL_reg_name[OP(o)]); /* Take off const! */

    k = PL_regkind[OP(o)];

    if (k == EXACT) {
	sv_catpvs(sv, " ");
	/* Using is_utf8_string() (via PERL_PV_UNI_DETECT)
	 * is a crude hack but it may be the best for now since
	 * we have no flag "this EXACTish node was UTF-8"
	 * --jhi */
	pv_pretty(sv, STRING(o), STR_LEN(o), PL_dump_re_max_len,
                  PL_colors[0], PL_colors[1],
		  PERL_PV_ESCAPE_UNI_DETECT |
		  PERL_PV_ESCAPE_NONASCII   |
		  PERL_PV_PRETTY_ELLIPSES   |
		  PERL_PV_PRETTY_LTGT       |
		  PERL_PV_PRETTY_NOCLEAR
		  );
    } else if (k == TRIE) {
	/* print the details of the trie in dumpuntil instead, as
	 * progi->data isn't available here */
        const char op = OP(o);
        const U32 n = ARG(o);
        const reg_ac_data * const ac = IS_TRIE_AC(op) ?
               (reg_ac_data *)progi->data->data[n] :
               NULL;
        const reg_trie_data * const trie
	    = (reg_trie_data*)progi->data->data[!IS_TRIE_AC(op) ? n : ac->trie];

        Perl_sv_catpvf(aTHX_ sv, "-%s", PL_reg_name[o->flags]);
        DEBUG_TRIE_COMPILE_r({
          if (trie->jump)
            sv_catpvs(sv, "(JUMP)");
          Perl_sv_catpvf(aTHX_ sv,
            "<S:%" UVuf "/%" IVdf " W:%" UVuf " L:%" UVuf "/%" UVuf " C:%" UVuf "/%" UVuf ">",
            (UV)trie->startstate,
            (IV)trie->statecount-1, /* -1 because of the unused 0 element */
            (UV)trie->wordcount,
            (UV)trie->minlen,
            (UV)trie->maxlen,
            (UV)TRIE_CHARCOUNT(trie),
            (UV)trie->uniquecharcount
          );
        });
        if ( IS_ANYOF_TRIE(op) || trie->bitmap ) {
            sv_catpvs(sv, "[");
            (void) put_charclass_bitmap_innards(sv,
                                                ((IS_ANYOF_TRIE(op))
                                                 ? ANYOF_BITMAP(o)
                                                 : TRIE_BITMAP(trie)),
                                                NULL,
                                                NULL,
                                                NULL,
                                                FALSE
                                               );
            sv_catpvs(sv, "]");
        }
    } else if (k == CURLY) {
        U32 lo = ARG1(o), hi = ARG2(o);
	if (OP(o) == CURLYM || OP(o) == CURLYN || OP(o) == CURLYX)
	    Perl_sv_catpvf(aTHX_ sv, "[%d]", o->flags); /* Parenth number */
        Perl_sv_catpvf(aTHX_ sv, "{%u,", (unsigned) lo);
        if (hi == REG_INFTY)
            sv_catpvs(sv, "INFTY");
        else
            Perl_sv_catpvf(aTHX_ sv, "%u", (unsigned) hi);
        sv_catpvs(sv, "}");
    }
    else if (k == WHILEM && o->flags)			/* Ordinal/of */
	Perl_sv_catpvf(aTHX_ sv, "[%d/%d]", o->flags & 0xf, o->flags>>4);
    else if (k == REF || k == OPEN || k == CLOSE
             || k == GROUPP || OP(o)==ACCEPT)
    {
        AV *name_list= NULL;
        U32 parno= OP(o) == ACCEPT ? (U32)ARG2L(o) : ARG(o);
        Perl_sv_catpvf(aTHX_ sv, "%" UVuf, (UV)parno);        /* Parenth number */
	if ( RXp_PAREN_NAMES(prog) ) {
            name_list= MUTABLE_AV(progi->data->data[progi->name_list_idx]);
        } else if ( pRExC_state ) {
            name_list= RExC_paren_name_list;
        }
        if (name_list) {
            if ( k != REF || (OP(o) < NREF)) {
                SV **name= av_fetch(name_list, parno, 0 );
	        if (name)
	            Perl_sv_catpvf(aTHX_ sv, " '%" SVf "'", SVfARG(*name));
            }
            else {
                SV *sv_dat= MUTABLE_SV(progi->data->data[ parno ]);
                I32 *nums=(I32*)SvPVX(sv_dat);
                SV **name= av_fetch(name_list, nums[0], 0 );
                I32 n;
                if (name) {
                    for ( n=0; n<SvIVX(sv_dat); n++ ) {
                        Perl_sv_catpvf(aTHX_ sv, "%s%" IVdf,
			   	    (n ? "," : ""), (IV)nums[n]);
                    }
                    Perl_sv_catpvf(aTHX_ sv, " '%" SVf "'", SVfARG(*name));
                }
            }
        }
        if ( k == REF && reginfo) {
            U32 n = ARG(o);  /* which paren pair */
            I32 ln = prog->offs[n].start;
            if (prog->lastparen < n || ln == -1 || prog->offs[n].end == -1)
                Perl_sv_catpvf(aTHX_ sv, ": FAIL");
            else if (ln == prog->offs[n].end)
                Perl_sv_catpvf(aTHX_ sv, ": ACCEPT - EMPTY STRING");
            else {
                const char *s = reginfo->strbeg + ln;
                Perl_sv_catpvf(aTHX_ sv, ": ");
                Perl_pv_pretty( aTHX_ sv, s, prog->offs[n].end - prog->offs[n].start, 32, 0, 0,
                    PERL_PV_ESCAPE_UNI_DETECT|PERL_PV_PRETTY_NOCLEAR|PERL_PV_PRETTY_ELLIPSES|PERL_PV_PRETTY_QUOTE );
            }
        }
    } else if (k == GOSUB) {
        AV *name_list= NULL;
        if ( RXp_PAREN_NAMES(prog) ) {
            name_list= MUTABLE_AV(progi->data->data[progi->name_list_idx]);
        } else if ( pRExC_state ) {
            name_list= RExC_paren_name_list;
        }

        /* Paren and offset */
        Perl_sv_catpvf(aTHX_ sv, "%d[%+d:%d]", (int)ARG(o),(int)ARG2L(o),
                (int)((o + (int)ARG2L(o)) - progi->program) );
        if (name_list) {
            SV **name= av_fetch(name_list, ARG(o), 0 );
            if (name)
                Perl_sv_catpvf(aTHX_ sv, " '%" SVf "'", SVfARG(*name));
        }
    }
    else if (k == LOGICAL)
        /* 2: embedded, otherwise 1 */
	Perl_sv_catpvf(aTHX_ sv, "[%d]", o->flags);
    else if (k == ANYOF) {
	const U8 flags = (OP(o) == ANYOFH) ? 0 : ANYOF_FLAGS(o);
        bool do_sep = FALSE;    /* Do we need to separate various components of
                                   the output? */
        /* Set if there is still an unresolved user-defined property */
        SV *unresolved                = NULL;

        /* Things that are ignored except when the runtime locale is UTF-8 */
        SV *only_utf8_locale_invlist = NULL;

        /* Code points that don't fit in the bitmap */
        SV *nonbitmap_invlist = NULL;

        /* And things that aren't in the bitmap, but are small enough to be */
        SV* bitmap_range_not_in_bitmap = NULL;

        const bool inverted = flags & ANYOF_INVERT;

	if (OP(o) == ANYOFL || OP(o) == ANYOFPOSIXL) {
            if (ANYOFL_UTF8_LOCALE_REQD(flags)) {
                sv_catpvs(sv, "{utf8-locale-reqd}");
            }
            if (flags & ANYOFL_FOLD) {
                sv_catpvs(sv, "{i}");
            }
        }

        /* If there is stuff outside the bitmap, get it */
        if (ARG(o) != ANYOF_ONLY_HAS_BITMAP) {
            (void) _get_regclass_nonbitmap_data(prog, o, FALSE,
                                                &unresolved,
                                                &only_utf8_locale_invlist,
                                                &nonbitmap_invlist);
            /* The non-bitmap data may contain stuff that could fit in the
             * bitmap.  This could come from a user-defined property being
             * finally resolved when this call was done; or much more likely
             * because there are matches that require UTF-8 to be valid, and so
             * aren't in the bitmap.  This is teased apart later */
            _invlist_intersection(nonbitmap_invlist,
                                  PL_InBitmap,
                                  &bitmap_range_not_in_bitmap);
            /* Leave just the things that don't fit into the bitmap */
            _invlist_subtract(nonbitmap_invlist,
                              PL_InBitmap,
                              &nonbitmap_invlist);
        }

        /* Obey this flag to add all above-the-bitmap code points */
        if (flags & ANYOF_MATCHES_ALL_ABOVE_BITMAP) {
            nonbitmap_invlist = _add_range_to_invlist(nonbitmap_invlist,
                                                      NUM_ANYOF_CODE_POINTS,
                                                      UV_MAX);
        }

        /* Ready to start outputting.  First, the initial left bracket */
	Perl_sv_catpvf(aTHX_ sv, "[%s", PL_colors[0]);

        if (OP(o) != ANYOFH) {
            /* Then all the things that could fit in the bitmap */
            do_sep = put_charclass_bitmap_innards(sv,
                                                  ANYOF_BITMAP(o),
                                                  bitmap_range_not_in_bitmap,
                                                  only_utf8_locale_invlist,
                                                  o,

                                                  /* Can't try inverting for a
                                                   * better display if there
                                                   * are things that haven't
                                                   * been resolved */
                                                  unresolved != NULL);
            SvREFCNT_dec(bitmap_range_not_in_bitmap);

            /* If there are user-defined properties which haven't been defined
             * yet, output them.  If the result is not to be inverted, it is
             * clearest to output them in a separate [] from the bitmap range
             * stuff.  If the result is to be complemented, we have to show
             * everything in one [], as the inversion applies to the whole
             * thing.  Use {braces} to separate them from anything in the
             * bitmap and anything above the bitmap. */
            if (unresolved) {
                if (inverted) {
                    if (! do_sep) { /* If didn't output anything in the bitmap
                                     */
                        sv_catpvs(sv, "^");
                    }
                    sv_catpvs(sv, "{");
                }
                else if (do_sep) {
                    Perl_sv_catpvf(aTHX_ sv,"%s][%s", PL_colors[1],
                                                      PL_colors[0]);
                }
                sv_catsv(sv, unresolved);
                if (inverted) {
                    sv_catpvs(sv, "}");
                }
                do_sep = ! inverted;
            }
        }

        /* And, finally, add the above-the-bitmap stuff */
        if (nonbitmap_invlist && _invlist_len(nonbitmap_invlist)) {
            SV* contents;

            /* See if truncation size is overridden */
            const STRLEN dump_len = (PL_dump_re_max_len > 256)
                                    ? PL_dump_re_max_len
                                    : 256;

            /* This is output in a separate [] */
            if (do_sep) {
                Perl_sv_catpvf(aTHX_ sv,"%s][%s", PL_colors[1], PL_colors[0]);
            }

            /* And, for easy of understanding, it is shown in the
             * uncomplemented form if possible.  The one exception being if
             * there are unresolved items, where the inversion has to be
             * delayed until runtime */
            if (inverted && ! unresolved) {
                _invlist_invert(nonbitmap_invlist);
                _invlist_subtract(nonbitmap_invlist, PL_InBitmap, &nonbitmap_invlist);
            }

            contents = invlist_contents(nonbitmap_invlist,
                                        FALSE /* output suitable for catsv */
                                       );

            /* If the output is shorter than the permissible maximum, just do it. */
            if (SvCUR(contents) <= dump_len) {
                sv_catsv(sv, contents);
            }
            else {
                const char * contents_string = SvPVX(contents);
                STRLEN i = dump_len;

                /* Otherwise, start at the permissible max and work back to the
                 * first break possibility */
                while (i > 0 && contents_string[i] != ' ') {
                    i--;
                }
                if (i == 0) {       /* Fail-safe.  Use the max if we couldn't
                                       find a legal break */
                    i = dump_len;
                }

                sv_catpvn(sv, contents_string, i);
                sv_catpvs(sv, "...");
            }

            SvREFCNT_dec_NN(contents);
            SvREFCNT_dec_NN(nonbitmap_invlist);
        }

        /* And finally the matching, closing ']' */
	Perl_sv_catpvf(aTHX_ sv, "%s]", PL_colors[1]);

        if (OP(o) == ANYOFH && FLAGS(o) != 0) {
            Perl_sv_catpvf(aTHX_ sv, " (First UTF-8 byte=\\x%02x)", FLAGS(o));
        }


        SvREFCNT_dec(unresolved);
    }
    else if (k == ANYOFM) {
        SV * cp_list = get_ANYOFM_contents(o);

	Perl_sv_catpvf(aTHX_ sv, "[%s", PL_colors[0]);
        if (OP(o) == NANYOFM) {
            _invlist_invert(cp_list);
        }

        put_charclass_bitmap_innards(sv, NULL, cp_list, NULL, NULL, TRUE);
	Perl_sv_catpvf(aTHX_ sv, "%s]", PL_colors[1]);

        SvREFCNT_dec(cp_list);
    }
    else if (k == POSIXD || k == NPOSIXD) {
        U8 index = FLAGS(o) * 2;
        if (index < C_ARRAY_LENGTH(anyofs)) {
            if (*anyofs[index] != '[')  {
                sv_catpvs(sv, "[");
            }
            sv_catpv(sv, anyofs[index]);
            if (*anyofs[index] != '[')  {
                sv_catpvs(sv, "]");
            }
        }
        else {
            Perl_sv_catpvf(aTHX_ sv, "[illegal type=%d])", index);
        }
    }
    else if (k == BOUND || k == NBOUND) {
        /* Must be synced with order of 'bound_type' in regcomp.h */
        const char * const bounds[] = {
            "",      /* Traditional */
            "{gcb}",
            "{lb}",
            "{sb}",
            "{wb}"
        };
        assert(FLAGS(o) < C_ARRAY_LENGTH(bounds));
        sv_catpv(sv, bounds[FLAGS(o)]);
    }
    else if (k == BRANCHJ && (OP(o) == UNLESSM || OP(o) == IFMATCH)) {
	Perl_sv_catpvf(aTHX_ sv, "[%d", -(o->flags));
        if (o->next_off) {
            Perl_sv_catpvf(aTHX_ sv, "..-%d", o->flags - o->next_off);
        }
	Perl_sv_catpvf(aTHX_ sv, "]");
    }
    else if (OP(o) == SBOL)
        Perl_sv_catpvf(aTHX_ sv, " /%s/", o->flags ? "\\A" : "^");

    /* add on the verb argument if there is one */
    if ( ( k == VERB || OP(o) == ACCEPT || OP(o) == OPFAIL ) && o->flags) {
        if ( ARG(o) )
            Perl_sv_catpvf(aTHX_ sv, ":%" SVf,
                       SVfARG((MUTABLE_SV(progi->data->data[ ARG( o ) ]))));
        else
            sv_catpvs(sv, ":NULL");
    }
#else
    PERL_UNUSED_CONTEXT;
    PERL_UNUSED_ARG(sv);
    PERL_UNUSED_ARG(o);
    PERL_UNUSED_ARG(prog);
    PERL_UNUSED_ARG(reginfo);
    PERL_UNUSED_ARG(pRExC_state);
#endif	/* DEBUGGING */