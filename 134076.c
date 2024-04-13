smacro_expand_default(const SMacro *s, Token **params, int nparams)
{
    (void)params;
    (void)nparams;

    return dup_tlist(s->expansion, NULL);
}