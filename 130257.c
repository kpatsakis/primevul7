smacro_defined(Context * ctx, const char *name, int nparam, SMacro ** defn,
               bool nocase)
{
    struct hash_table *smtbl;
    SMacro *m;

    if (ctx) {
        smtbl = &ctx->localmac;
    } else if (name[0] == '%' && name[1] == '$') {
        if (cstk)
            ctx = get_ctx(name, &name);
        if (!ctx)
            return false;       /* got to return _something_ */
        smtbl = &ctx->localmac;
    } else {
        smtbl = &smacros;
    }
    m = (SMacro *) hash_findix(smtbl, name);

    while (m) {
        if (!mstrcmp(m->name, name, m->casesense && nocase) &&
            (nparam <= 0 || m->nparam == 0 || nparam == (int) m->nparam)) {
            if (defn) {
                if (nparam == (int) m->nparam || nparam == -1)
                    *defn = m;
                else
                    *defn = NULL;
            }
            return true;
        }
        m = m->next;
    }

    return false;
}