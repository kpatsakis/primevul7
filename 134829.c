STATIC SV *
S_get_fq_name(pTHX_
              const char * const name,    /* The first non-blank in the \p{}, \P{} */
              const Size_t name_len,      /* Its length in bytes, not including any trailing space */
              const bool is_utf8,         /* ? Is 'name' encoded in UTF-8 */
              const bool has_colon_colon
             )
{
    /* Returns a mortal SV containing the fully qualified version of the input
     * name */

    SV * fq_name;

    fq_name = newSVpvs_flags("", SVs_TEMP);

    /* Use the current package if it wasn't included in our input */
    if (! has_colon_colon) {
        const HV * pkg = (IN_PERL_COMPILETIME)
                         ? PL_curstash
                         : CopSTASH(PL_curcop);
        const char* pkgname = HvNAME(pkg);

        Perl_sv_catpvf(aTHX_ fq_name, "%" UTF8f,
                      UTF8fARG(is_utf8, strlen(pkgname), pkgname));
        sv_catpvs(fq_name, "::");
    }

    Perl_sv_catpvf(aTHX_ fq_name, "%" UTF8f,
                         UTF8fARG(is_utf8, name_len, name));
    return fq_name;