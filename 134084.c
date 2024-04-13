stdmac_bits(const SMacro *s, Token **params, int nparams)
{
    (void)s;
    (void)params;
    (void)nparams;

    return make_tok_num(NULL, globalbits);
}