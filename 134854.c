STATIC void
S_put_charclass_bitmap_innards_invlist(pTHX_ SV *sv, SV* invlist)
{
    /* Concatenate onto the PV in 'sv' a displayable form of the inversion list
     * 'invlist' */

    UV start, end;
    bool allow_literals = TRUE;

    PERL_ARGS_ASSERT_PUT_CHARCLASS_BITMAP_INNARDS_INVLIST;

    /* Generally, it is more readable if printable characters are output as
     * literals, but if a range (nearly) spans all of them, it's best to output
     * it as a single range.  This code will use a single range if all but 2
     * ASCII printables are in it */
    invlist_iterinit(invlist);
    while (invlist_iternext(invlist, &start, &end)) {

        /* If the range starts beyond the final printable, it doesn't have any
         * in it */
        if (start > MAX_PRINT_A) {
            break;
        }

        /* In both ASCII and EBCDIC, a SPACE is the lowest printable.  To span
         * all but two, the range must start and end no later than 2 from
         * either end */
        if (start < ' ' + 2 && end > MAX_PRINT_A - 2) {
            if (end > MAX_PRINT_A) {
                end = MAX_PRINT_A;
            }
            if (start < ' ') {
                start = ' ';
            }
            if (end - start >= MAX_PRINT_A - ' ' - 2) {
                allow_literals = FALSE;
            }
            break;
        }
    }
    invlist_iterfinish(invlist);

    /* Here we have figured things out.  Output each range */
    invlist_iterinit(invlist);
    while (invlist_iternext(invlist, &start, &end)) {
        if (start >= NUM_ANYOF_CODE_POINTS) {
            break;
        }
        put_range(sv, start, end, allow_literals);
    }
    invlist_iterfinish(invlist);

    return;