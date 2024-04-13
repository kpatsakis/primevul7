stdmac_file(const SMacro *s, Token **params, int nparams)
{
    (void)s;
    (void)params;
    (void)nparams;

    return make_tok_qstr(NULL, src_get_fname());
}