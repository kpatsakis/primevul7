STATIC void
S_put_range(pTHX_ SV *sv, UV start, const UV end, const bool allow_literals)
{
    /* Appends to 'sv' a displayable version of the range of code points from
     * 'start' to 'end'.  Mnemonics (like '\r') are used for the few controls
     * that have them, when they occur at the beginning or end of the range.
     * It uses hex to output the remaining code points, unless 'allow_literals'
     * is true, in which case the printable ASCII ones are output as-is (though
     * some of these will be escaped by put_code_point()).
     *
     * NOTE:  This is designed only for printing ranges of code points that fit
     *        inside an ANYOF bitmap.  Higher code points are simply suppressed
     */

    const unsigned int min_range_count = 3;

    assert(start <= end);

    PERL_ARGS_ASSERT_PUT_RANGE;

    while (start <= end) {
        UV this_end;
        const char * format;

        if (end - start < min_range_count) {

            /* Output chars individually when they occur in short ranges */
            for (; start <= end; start++) {
                put_code_point(sv, start);
            }
            break;
        }

        /* If permitted by the input options, and there is a possibility that
         * this range contains a printable literal, look to see if there is
         * one. */
        if (allow_literals && start <= MAX_PRINT_A) {

            /* If the character at the beginning of the range isn't an ASCII
             * printable, effectively split the range into two parts:
             *  1) the portion before the first such printable,
             *  2) the rest
             * and output them separately. */
            if (! isPRINT_A(start)) {
                UV temp_end = start + 1;

                /* There is no point looking beyond the final possible
                 * printable, in MAX_PRINT_A */
                UV max = MIN(end, MAX_PRINT_A);

                while (temp_end <= max && ! isPRINT_A(temp_end)) {
                    temp_end++;
                }

                /* Here, temp_end points to one beyond the first printable if
                 * found, or to one beyond 'max' if not.  If none found, make
                 * sure that we use the entire range */
                if (temp_end > MAX_PRINT_A) {
                    temp_end = end + 1;
                }

                /* Output the first part of the split range: the part that
                 * doesn't have printables, with the parameter set to not look
                 * for literals (otherwise we would infinitely recurse) */
                put_range(sv, start, temp_end - 1, FALSE);

                /* The 2nd part of the range (if any) starts here. */
                start = temp_end;

                /* We do a continue, instead of dropping down, because even if
                 * the 2nd part is non-empty, it could be so short that we want
                 * to output it as individual characters, as tested for at the
                 * top of this loop.  */
                continue;
            }

            /* Here, 'start' is a printable ASCII.  If it is an alphanumeric,
             * output a sub-range of just the digits or letters, then process
             * the remaining portion as usual. */
            if (isALPHANUMERIC_A(start)) {
                UV mask = (isDIGIT_A(start))
                           ? _CC_DIGIT
                             : isUPPER_A(start)
                               ? _CC_UPPER
                               : _CC_LOWER;
                UV temp_end = start + 1;

                /* Find the end of the sub-range that includes just the
                 * characters in the same class as the first character in it */
                while (temp_end <= end && _generic_isCC_A(temp_end, mask)) {
                    temp_end++;
                }
                temp_end--;

                /* For short ranges, don't duplicate the code above to output
                 * them; just call recursively */
                if (temp_end - start < min_range_count) {
                    put_range(sv, start, temp_end, FALSE);
                }
                else {  /* Output as a range */
                    put_code_point(sv, start);
                    sv_catpvs(sv, "-");
                    put_code_point(sv, temp_end);
                }
                start = temp_end + 1;
                continue;
            }

            /* We output any other printables as individual characters */
            if (isPUNCT_A(start) || isSPACE_A(start)) {
                while (start <= end && (isPUNCT_A(start)
                                        || isSPACE_A(start)))
                {
                    put_code_point(sv, start);
                    start++;
                }
                continue;
            }
        } /* End of looking for literals */

        /* Here is not to output as a literal.  Some control characters have
         * mnemonic names.  Split off any of those at the beginning and end of
         * the range to print mnemonically.  It isn't possible for many of
         * these to be in a row, so this won't overwhelm with output */
        if (   start <= end
            && (isMNEMONIC_CNTRL(start) || isMNEMONIC_CNTRL(end)))
        {
            while (isMNEMONIC_CNTRL(start) && start <= end) {
                put_code_point(sv, start);
                start++;
            }

            /* If this didn't take care of the whole range ... */
            if (start <= end) {

                /* Look backwards from the end to find the final non-mnemonic
                 * */
                UV temp_end = end;
                while (isMNEMONIC_CNTRL(temp_end)) {
                    temp_end--;
                }

                /* And separately output the interior range that doesn't start
                 * or end with mnemonics */
                put_range(sv, start, temp_end, FALSE);

                /* Then output the mnemonic trailing controls */
                start = temp_end + 1;
                while (start <= end) {
                    put_code_point(sv, start);
                    start++;
                }
                break;
            }
        }

        /* As a final resort, output the range or subrange as hex. */

        this_end = (end < NUM_ANYOF_CODE_POINTS)
                    ? end
                    : NUM_ANYOF_CODE_POINTS - 1;
#if NUM_ANYOF_CODE_POINTS > 256
        format = (this_end < 256)
                 ? "\\x%02" UVXf "-\\x%02" UVXf
                 : "\\x{%04" UVXf "}-\\x{%04" UVXf "}";
#else
        format = "\\x%02" UVXf "-\\x%02" UVXf;
#endif
        GCC_DIAG_IGNORE_STMT(-Wformat-nonliteral);
        Perl_sv_catpvf(aTHX_ sv, format, start, this_end);
        GCC_DIAG_RESTORE_STMT;
        break;
    }