STATIC regnode_offset
S_handle_regex_sets(pTHX_ RExC_state_t *pRExC_state, SV** return_invlist,
                    I32 *flagp, U32 depth,
                    char * const oregcomp_parse)
{
    /* Handle the (?[...]) construct to do set operations */

    U8 curchar;                     /* Current character being parsed */
    UV start, end;	            /* End points of code point ranges */
    SV* final = NULL;               /* The end result inversion list */
    SV* result_string;              /* 'final' stringified */
    AV* stack;                      /* stack of operators and operands not yet
                                       resolved */
    AV* fence_stack = NULL;         /* A stack containing the positions in
                                       'stack' of where the undealt-with left
                                       parens would be if they were actually
                                       put there */
    /* The 'volatile' is a workaround for an optimiser bug
     * in Solaris Studio 12.3. See RT #127455 */
    volatile IV fence = 0;          /* Position of where most recent undealt-
                                       with left paren in stack is; -1 if none.
                                     */
    STRLEN len;                     /* Temporary */
    regnode_offset node;                  /* Temporary, and final regnode returned by
                                       this function */
    const bool save_fold = FOLD;    /* Temporary */
    char *save_end, *save_parse;    /* Temporaries */
    const bool in_locale = LOC;     /* we turn off /l during processing */

    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_HANDLE_REGEX_SETS;

    DEBUG_PARSE("xcls");

    if (in_locale) {
        set_regex_charset(&RExC_flags, REGEX_UNICODE_CHARSET);
    }

    /* The use of this operator implies /u.  This is required so that the
     * compile time values are valid in all runtime cases */
    REQUIRE_UNI_RULES(flagp, 0);

    ckWARNexperimental(RExC_parse,
                       WARN_EXPERIMENTAL__REGEX_SETS,
                       "The regex_sets feature is experimental");

    /* Everything in this construct is a metacharacter.  Operands begin with
     * either a '\' (for an escape sequence), or a '[' for a bracketed
     * character class.  Any other character should be an operator, or
     * parenthesis for grouping.  Both types of operands are handled by calling
     * regclass() to parse them.  It is called with a parameter to indicate to
     * return the computed inversion list.  The parsing here is implemented via
     * a stack.  Each entry on the stack is a single character representing one
     * of the operators; or else a pointer to an operand inversion list. */

#define IS_OPERATOR(a) SvIOK(a)
#define IS_OPERAND(a)  (! IS_OPERATOR(a))

    /* The stack is kept in Łukasiewicz order.  (That's pronounced similar
     * to luke-a-shave-itch (or -itz), but people who didn't want to bother
     * with pronouncing it called it Reverse Polish instead, but now that YOU
     * know how to pronounce it you can use the correct term, thus giving due
     * credit to the person who invented it, and impressing your geek friends.
     * Wikipedia says that the pronounciation of "Ł" has been changing so that
     * it is now more like an English initial W (as in wonk) than an L.)
     *
     * This means that, for example, 'a | b & c' is stored on the stack as
     *
     * c  [4]
     * b  [3]
     * &  [2]
     * a  [1]
     * |  [0]
     *
     * where the numbers in brackets give the stack [array] element number.
     * In this implementation, parentheses are not stored on the stack.
     * Instead a '(' creates a "fence" so that the part of the stack below the
     * fence is invisible except to the corresponding ')' (this allows us to
     * replace testing for parens, by using instead subtraction of the fence
     * position).  As new operands are processed they are pushed onto the stack
     * (except as noted in the next paragraph).  New operators of higher
     * precedence than the current final one are inserted on the stack before
     * the lhs operand (so that when the rhs is pushed next, everything will be
     * in the correct positions shown above.  When an operator of equal or
     * lower precedence is encountered in parsing, all the stacked operations
     * of equal or higher precedence are evaluated, leaving the result as the
     * top entry on the stack.  This makes higher precedence operations
     * evaluate before lower precedence ones, and causes operations of equal
     * precedence to left associate.
     *
     * The only unary operator '!' is immediately pushed onto the stack when
     * encountered.  When an operand is encountered, if the top of the stack is
     * a '!", the complement is immediately performed, and the '!' popped.  The
     * resulting value is treated as a new operand, and the logic in the
     * previous paragraph is executed.  Thus in the expression
     *      [a] + ! [b]
     * the stack looks like
     *
     * !
     * a
     * +
     *
     * as 'b' gets parsed, the latter gets evaluated to '!b', and the stack
     * becomes
     *
     * !b
     * a
     * +
     *
     * A ')' is treated as an operator with lower precedence than all the
     * aforementioned ones, which causes all operations on the stack above the
     * corresponding '(' to be evaluated down to a single resultant operand.
     * Then the fence for the '(' is removed, and the operand goes through the
     * algorithm above, without the fence.
     *
     * A separate stack is kept of the fence positions, so that the position of
     * the latest so-far unbalanced '(' is at the top of it.
     *
     * The ']' ending the construct is treated as the lowest operator of all,
     * so that everything gets evaluated down to a single operand, which is the
     * result */

    sv_2mortal((SV *)(stack = newAV()));
    sv_2mortal((SV *)(fence_stack = newAV()));

    while (RExC_parse < RExC_end) {
        I32 top_index;              /* Index of top-most element in 'stack' */
        SV** top_ptr;               /* Pointer to top 'stack' element */
        SV* current = NULL;         /* To contain the current inversion list
                                       operand */
        SV* only_to_avoid_leaks;

        skip_to_be_ignored_text(pRExC_state, &RExC_parse,
                                TRUE /* Force /x */ );
        if (RExC_parse >= RExC_end) {   /* Fail */
            break;
        }

        curchar = UCHARAT(RExC_parse);

redo_curchar:

#ifdef ENABLE_REGEX_SETS_DEBUGGING
                    /* Enable with -Accflags=-DENABLE_REGEX_SETS_DEBUGGING */
        DEBUG_U(dump_regex_sets_structures(pRExC_state,
                                           stack, fence, fence_stack));
#endif

        top_index = av_tindex_skip_len_mg(stack);

        switch (curchar) {
            SV** stacked_ptr;       /* Ptr to something already on 'stack' */
            char stacked_operator;  /* The topmost operator on the 'stack'. */
            SV* lhs;                /* Operand to the left of the operator */
            SV* rhs;                /* Operand to the right of the operator */
            SV* fence_ptr;          /* Pointer to top element of the fence
                                       stack */

            case '(':

                if (   RExC_parse < RExC_end - 2
                    && UCHARAT(RExC_parse + 1) == '?'
                    && UCHARAT(RExC_parse + 2) == '^')
                {
                    /* If is a '(?', could be an embedded '(?^flags:(?[...])'.
                     * This happens when we have some thing like
                     *
                     *   my $thai_or_lao = qr/(?[ \p{Thai} + \p{Lao} ])/;
                     *   ...
                     *   qr/(?[ \p{Digit} & $thai_or_lao ])/;
                     *
                     * Here we would be handling the interpolated
                     * '$thai_or_lao'.  We handle this by a recursive call to
                     * ourselves which returns the inversion list the
                     * interpolated expression evaluates to.  We use the flags
                     * from the interpolated pattern. */
                    U32 save_flags = RExC_flags;
                    const char * save_parse;

                    RExC_parse += 2;        /* Skip past the '(?' */
                    save_parse = RExC_parse;

                    /* Parse the flags for the '(?'.  We already know the first
                     * flag to parse is a '^' */
                    parse_lparen_question_flags(pRExC_state);

                    if (   RExC_parse >= RExC_end - 4
                        || UCHARAT(RExC_parse) != ':'
                        || UCHARAT(++RExC_parse) != '('
                        || UCHARAT(++RExC_parse) != '?'
                        || UCHARAT(++RExC_parse) != '[')
                    {

                        /* In combination with the above, this moves the
                         * pointer to the point just after the first erroneous
                         * character. */
                        if (RExC_parse >= RExC_end - 4) {
                            RExC_parse = RExC_end;
                        }
                        else if (RExC_parse != save_parse) {
                            RExC_parse += (UTF)
                                          ? UTF8_SAFE_SKIP(RExC_parse, RExC_end)
                                          : 1;
                        }
                        vFAIL("Expecting '(?flags:(?[...'");
                    }

                    /* Recurse, with the meat of the embedded expression */
                    RExC_parse++;
                    if (! handle_regex_sets(pRExC_state, &current, flagp,
                                                    depth+1, oregcomp_parse))
                    {
                        RETURN_FAIL_ON_RESTART(*flagp, flagp);
                    }

                    /* Here, 'current' contains the embedded expression's
                     * inversion list, and RExC_parse points to the trailing
                     * ']'; the next character should be the ')' */
                    RExC_parse++;
                    if (UCHARAT(RExC_parse) != ')')
                        vFAIL("Expecting close paren for nested extended charclass");

                    /* Then the ')' matching the original '(' handled by this
                     * case: statement */
                    RExC_parse++;
                    if (UCHARAT(RExC_parse) != ')')
                        vFAIL("Expecting close paren for wrapper for nested extended charclass");

                    RExC_flags = save_flags;
                    goto handle_operand;
                }

                /* A regular '('.  Look behind for illegal syntax */
                if (top_index - fence >= 0) {
                    /* If the top entry on the stack is an operator, it had
                     * better be a '!', otherwise the entry below the top
                     * operand should be an operator */
                    if (   ! (top_ptr = av_fetch(stack, top_index, FALSE))
                        || (IS_OPERATOR(*top_ptr) && SvUV(*top_ptr) != '!')
                        || (   IS_OPERAND(*top_ptr)
                            && (   top_index - fence < 1
                                || ! (stacked_ptr = av_fetch(stack,
                                                             top_index - 1,
                                                             FALSE))
                                || ! IS_OPERATOR(*stacked_ptr))))
                    {
                        RExC_parse++;
                        vFAIL("Unexpected '(' with no preceding operator");
                    }
                }

                /* Stack the position of this undealt-with left paren */
                av_push(fence_stack, newSViv(fence));
                fence = top_index + 1;
                break;

            case '\\':
                /* regclass() can only return RESTART_PARSE and NEED_UTF8 if
                 * multi-char folds are allowed.  */
                if (!regclass(pRExC_state, flagp, depth+1,
                              TRUE, /* means parse just the next thing */
                              FALSE, /* don't allow multi-char folds */
                              FALSE, /* don't silence non-portable warnings.  */
                              TRUE,  /* strict */
                              FALSE, /* Require return to be an ANYOF */
                              &current))
                {
                    RETURN_FAIL_ON_RESTART(*flagp, flagp);
                    goto regclass_failed;
                }

                /* regclass() will return with parsing just the \ sequence,
                 * leaving the parse pointer at the next thing to parse */
                RExC_parse--;
                goto handle_operand;

            case '[':   /* Is a bracketed character class */
            {
                /* See if this is a [:posix:] class. */
                bool is_posix_class = (OOB_NAMEDCLASS
                            < handle_possible_posix(pRExC_state,
                                                RExC_parse + 1,
                                                NULL,
                                                NULL,
                                                TRUE /* checking only */));
                /* If it is a posix class, leave the parse pointer at the '['
                 * to fool regclass() into thinking it is part of a
                 * '[[:posix:]]'. */
                if (! is_posix_class) {
                    RExC_parse++;
                }

                /* regclass() can only return RESTART_PARSE and NEED_UTF8 if
                 * multi-char folds are allowed.  */
                if (!regclass(pRExC_state, flagp, depth+1,
                                is_posix_class, /* parse the whole char
                                                    class only if not a
                                                    posix class */
                                FALSE, /* don't allow multi-char folds */
                                TRUE, /* silence non-portable warnings. */
                                TRUE, /* strict */
                                FALSE, /* Require return to be an ANYOF */
                                &current))
                {
                    RETURN_FAIL_ON_RESTART(*flagp, flagp);
                    goto regclass_failed;
                }

                if (! current) {
                    break;
                }

                /* function call leaves parse pointing to the ']', except if we
                 * faked it */
                if (is_posix_class) {
                    RExC_parse--;
                }

                goto handle_operand;
            }

            case ']':
                if (top_index >= 1) {
                    goto join_operators;
                }

                /* Only a single operand on the stack: are done */
                goto done;

            case ')':
                if (av_tindex_skip_len_mg(fence_stack) < 0) {
                    if (UCHARAT(RExC_parse - 1) == ']')  {
                        break;
                    }
                    RExC_parse++;
                    vFAIL("Unexpected ')'");
                }

                /* If nothing after the fence, is missing an operand */
                if (top_index - fence < 0) {
                    RExC_parse++;
                    goto bad_syntax;
                }
                /* If at least two things on the stack, treat this as an
                  * operator */
                if (top_index - fence >= 1) {
                    goto join_operators;
                }

                /* Here only a single thing on the fenced stack, and there is a
                 * fence.  Get rid of it */
                fence_ptr = av_pop(fence_stack);
                assert(fence_ptr);
                fence = SvIV(fence_ptr);
                SvREFCNT_dec_NN(fence_ptr);
                fence_ptr = NULL;

                if (fence < 0) {
                    fence = 0;
                }

                /* Having gotten rid of the fence, we pop the operand at the
                 * stack top and process it as a newly encountered operand */
                current = av_pop(stack);
                if (IS_OPERAND(current)) {
                    goto handle_operand;
                }

                RExC_parse++;
                goto bad_syntax;

            case '&':
            case '|':
            case '+':
            case '-':
            case '^':

                /* These binary operators should have a left operand already
                 * parsed */
                if (   top_index - fence < 0
                    || top_index - fence == 1
                    || ( ! (top_ptr = av_fetch(stack, top_index, FALSE)))
                    || ! IS_OPERAND(*top_ptr))
                {
                    goto unexpected_binary;
                }

                /* If only the one operand is on the part of the stack visible
                 * to us, we just place this operator in the proper position */
                if (top_index - fence < 2) {

                    /* Place the operator before the operand */

                    SV* lhs = av_pop(stack);
                    av_push(stack, newSVuv(curchar));
                    av_push(stack, lhs);
                    break;
                }

                /* But if there is something else on the stack, we need to
                 * process it before this new operator if and only if the
                 * stacked operation has equal or higher precedence than the
                 * new one */

             join_operators:

                /* The operator on the stack is supposed to be below both its
                 * operands */
                if (   ! (stacked_ptr = av_fetch(stack, top_index - 2, FALSE))
                    || IS_OPERAND(*stacked_ptr))
                {
                    /* But if not, it's legal and indicates we are completely
                     * done if and only if we're currently processing a ']',
                     * which should be the final thing in the expression */
                    if (curchar == ']') {
                        goto done;
                    }

                  unexpected_binary:
                    RExC_parse++;
                    vFAIL2("Unexpected binary operator '%c' with no "
                           "preceding operand", curchar);
                }
                stacked_operator = (char) SvUV(*stacked_ptr);

                if (regex_set_precedence(curchar)
                    > regex_set_precedence(stacked_operator))
                {
                    /* Here, the new operator has higher precedence than the
                     * stacked one.  This means we need to add the new one to
                     * the stack to await its rhs operand (and maybe more
                     * stuff).  We put it before the lhs operand, leaving
                     * untouched the stacked operator and everything below it
                     * */
                    lhs = av_pop(stack);
                    assert(IS_OPERAND(lhs));

                    av_push(stack, newSVuv(curchar));
                    av_push(stack, lhs);
                    break;
                }

                /* Here, the new operator has equal or lower precedence than
                 * what's already there.  This means the operation already
                 * there should be performed now, before the new one. */

                rhs = av_pop(stack);
                if (! IS_OPERAND(rhs)) {

                    /* This can happen when a ! is not followed by an operand,
                     * like in /(?[\t &!])/ */
                    goto bad_syntax;
                }

                lhs = av_pop(stack);

                if (! IS_OPERAND(lhs)) {

                    /* This can happen when there is an empty (), like in
                     * /(?[[0]+()+])/ */
                    goto bad_syntax;
                }

                switch (stacked_operator) {
                    case '&':
                        _invlist_intersection(lhs, rhs, &rhs);
                        break;

                    case '|':
                    case '+':
                        _invlist_union(lhs, rhs, &rhs);
                        break;

                    case '-':
                        _invlist_subtract(lhs, rhs, &rhs);
                        break;

                    case '^':   /* The union minus the intersection */
                    {
                        SV* i = NULL;
                        SV* u = NULL;

                        _invlist_union(lhs, rhs, &u);
                        _invlist_intersection(lhs, rhs, &i);
                        _invlist_subtract(u, i, &rhs);
                        SvREFCNT_dec_NN(i);
                        SvREFCNT_dec_NN(u);
                        break;
                    }
                }
                SvREFCNT_dec(lhs);

                /* Here, the higher precedence operation has been done, and the
                 * result is in 'rhs'.  We overwrite the stacked operator with
                 * the result.  Then we redo this code to either push the new
                 * operator onto the stack or perform any higher precedence
                 * stacked operation */
                only_to_avoid_leaks = av_pop(stack);
                SvREFCNT_dec(only_to_avoid_leaks);
                av_push(stack, rhs);
                goto redo_curchar;

            case '!':   /* Highest priority, right associative */

                /* If what's already at the top of the stack is another '!",
                 * they just cancel each other out */
                if (   (top_ptr = av_fetch(stack, top_index, FALSE))
                    && (IS_OPERATOR(*top_ptr) && SvUV(*top_ptr) == '!'))
                {
                    only_to_avoid_leaks = av_pop(stack);
                    SvREFCNT_dec(only_to_avoid_leaks);
                }
                else { /* Otherwise, since it's right associative, just push
                          onto the stack */
                    av_push(stack, newSVuv(curchar));
                }
                break;

            default:
                RExC_parse += (UTF) ? UTF8SKIP(RExC_parse) : 1;
                if (RExC_parse >= RExC_end) {
                    break;
                }
                vFAIL("Unexpected character");

          handle_operand:

            /* Here 'current' is the operand.  If something is already on the
             * stack, we have to check if it is a !.  But first, the code above
             * may have altered the stack in the time since we earlier set
             * 'top_index'.  */

            top_index = av_tindex_skip_len_mg(stack);
            if (top_index - fence >= 0) {
                /* If the top entry on the stack is an operator, it had better
                 * be a '!', otherwise the entry below the top operand should
                 * be an operator */
                top_ptr = av_fetch(stack, top_index, FALSE);
                assert(top_ptr);
                if (IS_OPERATOR(*top_ptr)) {

                    /* The only permissible operator at the top of the stack is
                     * '!', which is applied immediately to this operand. */
                    curchar = (char) SvUV(*top_ptr);
                    if (curchar != '!') {
                        SvREFCNT_dec(current);
                        vFAIL2("Unexpected binary operator '%c' with no "
                                "preceding operand", curchar);
                    }

                    _invlist_invert(current);

                    only_to_avoid_leaks = av_pop(stack);
                    SvREFCNT_dec(only_to_avoid_leaks);

                    /* And we redo with the inverted operand.  This allows
                     * handling multiple ! in a row */
                    goto handle_operand;
                }
                          /* Single operand is ok only for the non-binary ')'
                           * operator */
                else if ((top_index - fence == 0 && curchar != ')')
                         || (top_index - fence > 0
                             && (! (stacked_ptr = av_fetch(stack,
                                                           top_index - 1,
                                                           FALSE))
                                 || IS_OPERAND(*stacked_ptr))))
                {
                    SvREFCNT_dec(current);
                    vFAIL("Operand with no preceding operator");
                }
            }

            /* Here there was nothing on the stack or the top element was
             * another operand.  Just add this new one */
            av_push(stack, current);

        } /* End of switch on next parse token */

        RExC_parse += (UTF) ? UTF8SKIP(RExC_parse) : 1;
    } /* End of loop parsing through the construct */

    vFAIL("Syntax error in (?[...])");

  done:

    if (RExC_parse >= RExC_end || RExC_parse[1] != ')') {
        if (RExC_parse < RExC_end) {
            RExC_parse++;
        }

        vFAIL("Unexpected ']' with no following ')' in (?[...");
    }

    if (av_tindex_skip_len_mg(fence_stack) >= 0) {
        vFAIL("Unmatched (");
    }

    if (av_tindex_skip_len_mg(stack) < 0   /* Was empty */
        || ((final = av_pop(stack)) == NULL)
        || ! IS_OPERAND(final)
        || ! is_invlist(final)
        || av_tindex_skip_len_mg(stack) >= 0)  /* More left on stack */
    {
      bad_syntax:
        SvREFCNT_dec(final);
        vFAIL("Incomplete expression within '(?[ ])'");
    }

    /* Here, 'final' is the resultant inversion list from evaluating the
     * expression.  Return it if so requested */
    if (return_invlist) {
        *return_invlist = final;
        return END;
    }

    /* Otherwise generate a resultant node, based on 'final'.  regclass() is
     * expecting a string of ranges and individual code points */
    invlist_iterinit(final);
    result_string = newSVpvs("");
    while (invlist_iternext(final, &start, &end)) {
        if (start == end) {
            Perl_sv_catpvf(aTHX_ result_string, "\\x{%" UVXf "}", start);
        }
        else {
            Perl_sv_catpvf(aTHX_ result_string, "\\x{%" UVXf "}-\\x{%" UVXf "}",
                                                     start,          end);
        }
    }

    /* About to generate an ANYOF (or similar) node from the inversion list we
     * have calculated */
    save_parse = RExC_parse;
    RExC_parse = SvPV(result_string, len);
    save_end = RExC_end;
    RExC_end = RExC_parse + len;
    TURN_OFF_WARNINGS_IN_SUBSTITUTE_PARSE;

    /* We turn off folding around the call, as the class we have constructed
     * already has all folding taken into consideration, and we don't want
     * regclass() to add to that */
    RExC_flags &= ~RXf_PMf_FOLD;
    /* regclass() can only return RESTART_PARSE and NEED_UTF8 if multi-char
     * folds are allowed.  */
    node = regclass(pRExC_state, flagp, depth+1,
                    FALSE, /* means parse the whole char class */
                    FALSE, /* don't allow multi-char folds */
                    TRUE, /* silence non-portable warnings.  The above may very
                             well have generated non-portable code points, but
                             they're valid on this machine */
                    FALSE, /* similarly, no need for strict */
                    FALSE, /* Require return to be an ANYOF */
                    NULL
                );

    RESTORE_WARNINGS;
    RExC_parse = save_parse + 1;
    RExC_end = save_end;
    SvREFCNT_dec_NN(final);
    SvREFCNT_dec_NN(result_string);

    if (save_fold) {
        RExC_flags |= RXf_PMf_FOLD;
    }

    if (!node) {
        RETURN_FAIL_ON_RESTART(*flagp, flagp);
        goto regclass_failed;
    }

    /* Fix up the node type if we are in locale.  (We have pretended we are
     * under /u for the purposes of regclass(), as this construct will only
     * work under UTF-8 locales.  But now we change the opcode to be ANYOFL (so
     * as to cause any warnings about bad locales to be output in regexec.c),
     * and add the flag that indicates to check if not in a UTF-8 locale.  The
     * reason we above forbid optimization into something other than an ANYOF
     * node is simply to minimize the number of code changes in regexec.c.
     * Otherwise we would have to create new EXACTish node types and deal with
     * them.  This decision could be revisited should this construct become
     * popular.
     *
     * (One might think we could look at the resulting ANYOF node and suppress
     * the flag if everything is above 255, as those would be UTF-8 only,
     * but this isn't true, as the components that led to that result could
     * have been locale-affected, and just happen to cancel each other out
     * under UTF-8 locales.) */
    if (in_locale) {
        set_regex_charset(&RExC_flags, REGEX_LOCALE_CHARSET);

        assert(OP(REGNODE_p(node)) == ANYOF);

        OP(REGNODE_p(node)) = ANYOFL;
        ANYOF_FLAGS(REGNODE_p(node))
                |= ANYOFL_SHARED_UTF8_LOCALE_fold_HAS_MATCHES_nonfold_REQD;
    }

    nextchar(pRExC_state);
    Set_Node_Length(REGNODE_p(node), RExC_parse - oregcomp_parse + 1); /* MJD */
    return node;

  regclass_failed:
    FAIL2("panic: regclass returned failure to handle_sets, " "flags=%#" UVxf,
                                                                (UV) *flagp);