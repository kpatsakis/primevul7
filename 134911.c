STATIC void
S_add_above_Latin1_folds(pTHX_ RExC_state_t *pRExC_state, const U8 cp, SV** invlist)
{
    /* This adds the Latin1/above-Latin1 folding rules.
     *
     * This should be called only for a Latin1-range code points, cp, which is
     * known to be involved in a simple fold with other code points above
     * Latin1.  It would give false results if /aa has been specified.
     * Multi-char folds are outside the scope of this, and must be handled
     * specially. */

    PERL_ARGS_ASSERT_ADD_ABOVE_LATIN1_FOLDS;

    assert(HAS_NONLATIN1_SIMPLE_FOLD_CLOSURE(cp));

    /* The rules that are valid for all Unicode versions are hard-coded in */
    switch (cp) {
        case 'k':
        case 'K':
          *invlist =
             add_cp_to_invlist(*invlist, KELVIN_SIGN);
            break;
        case 's':
        case 'S':
          *invlist = add_cp_to_invlist(*invlist, LATIN_SMALL_LETTER_LONG_S);
            break;
        case MICRO_SIGN:
          *invlist = add_cp_to_invlist(*invlist, GREEK_CAPITAL_LETTER_MU);
          *invlist = add_cp_to_invlist(*invlist, GREEK_SMALL_LETTER_MU);
            break;
        case LATIN_CAPITAL_LETTER_A_WITH_RING_ABOVE:
        case LATIN_SMALL_LETTER_A_WITH_RING_ABOVE:
          *invlist = add_cp_to_invlist(*invlist, ANGSTROM_SIGN);
            break;
        case LATIN_SMALL_LETTER_Y_WITH_DIAERESIS:
          *invlist = add_cp_to_invlist(*invlist,
                                        LATIN_CAPITAL_LETTER_Y_WITH_DIAERESIS);
            break;

        default:    /* Other code points are checked against the data for the
                       current Unicode version */
          {
            Size_t folds_count;
            unsigned int first_fold;
            const unsigned int * remaining_folds;
            UV folded_cp;

            if (isASCII(cp)) {
                folded_cp = toFOLD(cp);
            }
            else {
                U8 dummy_fold[UTF8_MAXBYTES_CASE+1];
                Size_t dummy_len;
                folded_cp = _to_fold_latin1(cp, dummy_fold, &dummy_len, 0);
            }

            if (folded_cp > 255) {
                *invlist = add_cp_to_invlist(*invlist, folded_cp);
            }

            folds_count = _inverse_folds(folded_cp, &first_fold,
                                                    &remaining_folds);
            if (folds_count == 0) {

                /* Use deprecated warning to increase the chances of this being
                 * output */
                ckWARN2reg_d(RExC_parse,
                        "Perl folding rules are not up-to-date for 0x%02X;"
                        " please use the perlbug utility to report;", cp);
            }
            else {
                unsigned int i;

                if (first_fold > 255) {
                    *invlist = add_cp_to_invlist(*invlist, first_fold);
                }
                for (i = 0; i < folds_count - 1; i++) {
                    if (remaining_folds[i] > 255) {
                        *invlist = add_cp_to_invlist(*invlist,
                                                    remaining_folds[i]);
                    }
                }
            }
            break;
         }
    }