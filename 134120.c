static void undef_smacro(const char *mname, bool undefalias)
{
    SMacro **smhead, *s, **sp;
    struct hash_table *smtbl;
    Context *ctx;

    ctx = get_ctx(mname, &mname);
    smtbl = ctx ? &ctx->localmac : &smacros;
    smhead = (SMacro **)hash_findi(smtbl, mname, NULL);

    if (smhead) {
        /*
         * We now have a macro name... go hunt for it.
         */
        sp = smhead;
        while ((s = *sp) != NULL) {
            if (!mstrcmp(s->name, mname, s->casesense)) {
                if (s->alias && !undefalias) {
                    if (!ppopt.noaliases) {
                        if (s->in_progress) {
                            nasm_nonfatal("macro alias loop");
                        } else {
                            s->in_progress = true;
                            undef_smacro(tok_text(s->expansion), false);
                            s->in_progress = false;
                        }
                    }
                } else {
                    if (list_option('d'))
                        list_smacro_def(s->alias ? PP_UNDEFALIAS : PP_UNDEF,
                                        ctx, s);
                    *sp = s->next;
                    free_smacro(s);
                    continue;
                }
            }
            sp = &s->next;
        }
    }
}