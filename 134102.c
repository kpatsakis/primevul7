smacro_defined(Context *ctx, const char *name, int nparam, SMacro **defn,
               bool nocase, bool find_alias)
{
    struct hash_table *smtbl;
    SMacro *m;

    smtbl = ctx ? &ctx->localmac : &smacros;

restart:
    m = (SMacro *) hash_findix(smtbl, name);

    while (m) {
        if (!mstrcmp(m->name, name, m->casesense && nocase) &&
            (nparam <= 0 || m->nparam == 0 || nparam == m->nparam ||
             (m->greedy && nparam >= m->nparam-1))) {
            if (m->alias && !find_alias) {
                if (!ppopt.noaliases) {
                    name = tok_text(m->expansion);
                    goto restart;
                } else {
                    continue;
                }
            }
            if (defn)
                *defn = m;
            return true;
        }
        m = m->next;
    }

    return false;
}