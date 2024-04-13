STATIC void
S_dump_regex_sets_structures(pTHX_ RExC_state_t *pRExC_state,
                             AV * stack, const IV fence, AV * fence_stack)
{   /* Dumps the stacks in handle_regex_sets() */

    const SSize_t stack_top = av_tindex_skip_len_mg(stack);
    const SSize_t fence_stack_top = av_tindex_skip_len_mg(fence_stack);
    SSize_t i;

    PERL_ARGS_ASSERT_DUMP_REGEX_SETS_STRUCTURES;

    PerlIO_printf(Perl_debug_log, "\nParse position is:%s\n", RExC_parse);

    if (stack_top < 0) {
        PerlIO_printf(Perl_debug_log, "Nothing on stack\n");
    }
    else {
        PerlIO_printf(Perl_debug_log, "Stack: (fence=%d)\n", (int) fence);
        for (i = stack_top; i >= 0; i--) {
            SV ** element_ptr = av_fetch(stack, i, FALSE);
            if (! element_ptr) {
            }

            if (IS_OPERATOR(*element_ptr)) {
                PerlIO_printf(Perl_debug_log, "[%d]: %c\n",
                                            (int) i, (int) SvIV(*element_ptr));
            }
            else {
                PerlIO_printf(Perl_debug_log, "[%d] ", (int) i);
                sv_dump(*element_ptr);
            }
        }
    }

    if (fence_stack_top < 0) {
        PerlIO_printf(Perl_debug_log, "Nothing on fence_stack\n");
    }
    else {
        PerlIO_printf(Perl_debug_log, "Fence_stack: \n");
        for (i = fence_stack_top; i >= 0; i--) {
            SV ** element_ptr = av_fetch(fence_stack, i, FALSE);
            if (! element_ptr) {
            }

            PerlIO_printf(Perl_debug_log, "[%d]: %d\n",
                                            (int) i, (int) SvIV(*element_ptr));
        }
    }