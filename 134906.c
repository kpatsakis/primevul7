STATIC void
S_put_code_point(pTHX_ SV *sv, UV c)
{
    PERL_ARGS_ASSERT_PUT_CODE_POINT;

    if (c > 255) {
        Perl_sv_catpvf(aTHX_ sv, "\\x{%04" UVXf "}", c);
    }
    else if (isPRINT(c)) {
	const char string = (char) c;

        /* We use {phrase} as metanotation in the class, so also escape literal
         * braces */
	if (isBACKSLASHED_PUNCT(c) || c == '{' || c == '}')
	    sv_catpvs(sv, "\\");
	sv_catpvn(sv, &string, 1);
    }
    else if (isMNEMONIC_CNTRL(c)) {
        Perl_sv_catpvf(aTHX_ sv, "%s", cntrl_to_mnemonic((U8) c));
    }
    else {
        Perl_sv_catpvf(aTHX_ sv, "\\x%02X", (U8) c);
    }