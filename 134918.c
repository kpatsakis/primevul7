STATIC SV*
S_get_ANYOFM_contents(pTHX_ const regnode * n) {

    /* Returns an inversion list of all the code points matched by the
     * ANYOFM/NANYOFM node 'n' */

    SV * cp_list = _new_invlist(-1);
    const U8 lowest = (U8) ARG(n);
    unsigned int i;
    U8 count = 0;
    U8 needed = 1U << PL_bitcount[ (U8) ~ FLAGS(n)];

    PERL_ARGS_ASSERT_GET_ANYOFM_CONTENTS;

    /* Starting with the lowest code point, any code point that ANDed with the
     * mask yields the lowest code point is in the set */
    for (i = lowest; i <= 0xFF; i++) {
        if ((i & FLAGS(n)) == ARG(n)) {
            cp_list = add_cp_to_invlist(cp_list, i);
            count++;

            /* We know how many code points (a power of two) that are in the
             * set.  No use looking once we've got that number */
            if (count >= needed) break;
        }
    }

    if (OP(n) == NANYOFM) {
        _invlist_invert(cp_list);
    }
    return cp_list;