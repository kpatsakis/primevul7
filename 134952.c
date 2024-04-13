STATIC void
S_output_posix_warnings(pTHX_ RExC_state_t *pRExC_state, AV* posix_warnings)
{
    /* Output the elements of the array given by '*posix_warnings' as REGEXP
     * warnings. */

    SV * msg;
    const bool first_is_fatal = ckDEAD(packWARN(WARN_REGEXP));

    PERL_ARGS_ASSERT_OUTPUT_POSIX_WARNINGS;

    if (! TO_OUTPUT_WARNINGS(RExC_parse)) {
        return;
    }

    while ((msg = av_shift(posix_warnings)) != &PL_sv_undef) {
        if (first_is_fatal) {           /* Avoid leaking this */
            av_undef(posix_warnings);   /* This isn't necessary if the
                                            array is mortal, but is a
                                            fail-safe */
            (void) sv_2mortal(msg);
            PREPARE_TO_DIE;
        }
        Perl_warner(aTHX_ packWARN(WARN_REGEXP), "%s", SvPVX(msg));
        SvREFCNT_dec_NN(msg);
    }

    UPDATE_WARNINGS_LOC(RExC_parse);