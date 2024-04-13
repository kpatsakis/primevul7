static SMacro *define_smacro(const char *mname, bool casesense,
                             Token *expansion, SMacro *tmpl)
{
    SMacro *smac, **smhead;
    struct hash_table *smtbl;
    Context *ctx;
    bool defining_alias = false;
    int nparam = 0;
    bool defined;

    if (tmpl) {
        defining_alias = tmpl->alias;
        nparam = tmpl->nparam;
        if (nparam && !defining_alias)
            mark_smac_params(expansion, tmpl, 0);
    }

    ctx = get_ctx(mname, &mname);

    defined = smacro_defined(ctx, mname, nparam, &smac, casesense, true);

    if (defined) {
        if (smac->alias) {
            if (smac->in_progress) {
                nasm_nonfatal("macro alias loop");
                goto fail;
            }

            if (!defining_alias && !ppopt.noaliases) {
                /* It is an alias macro; follow the alias link */
                SMacro *s;

                smac->in_progress = true;
                s = define_smacro(tok_text(smac->expansion), casesense,
                                  expansion, tmpl);
                smac->in_progress = false;
                return s;
            }
        }

        if (casesense ^ smac->casesense) {
            /*
             *!macro-def-case-single [on] single-line macro defined both case sensitive and insensitive
             *!  warns when a single-line macro is defined both case
             *!  sensitive and case insensitive.
             *!  The new macro
             *!  definition will override (shadow) the original one,
             *!  although the original macro is not deleted, and will
             *!  be re-exposed if the new macro is deleted with
             *!  \c{%undef}, or, if the original macro is the case
             *!  insensitive one, the macro call is done with a
             *!  different case.
             */
            nasm_warn(WARN_MACRO_DEF_CASE_SINGLE, "case %ssensitive definition of macro `%s' will shadow %ssensitive macro `%s'",
                      casesense ? "" : "in",
                      mname,
                      smac->casesense ? "" : "in",
                      smac->name);
            defined = false;
        } else if ((!!nparam) ^ (!!smac->nparam)) {
            /*
             * Most recent versions of NASM considered this an error,
             * so promote this warning to error by default.
             *
             *!macro-def-param-single [err] single-line macro defined with and without parameters
             *!  warns if the same single-line macro is defined with and
             *!  without parameters.
             *!  The new macro
             *!  definition will override (shadow) the original one,
             *!  although the original macro is not deleted, and will
             *!  be re-exposed if the new macro is deleted with
             *!  \c{%undef}.
             */
            nasm_warn(WARN_MACRO_DEF_PARAM_SINGLE,
                      "macro `%s' defined both with and without parameters",
                      mname);
            defined = false;
        } else if (smac->nparam < nparam) {
            /*
             *!macro-def-greedy-single [on] single-line macro
             *!  definition shadows greedy macro warns when a
             *!  single-line macro is defined which would match a
             *!  previously existing greedy definition.  The new macro
             *!  definition will override (shadow) the original one,
             *!  although the original macro is not deleted, and will
             *!  be re-exposed if the new macro is deleted with
             *!  \c{%undef}, and will be invoked if called with a
             *!  parameter count that does not match the new definition.
             */
            nasm_warn(WARN_MACRO_DEF_GREEDY_SINGLE,
                      "defining macro `%s' shadows previous greedy definition",
                      mname);
            defined = false;
        }
    }

    if (defined) {
        /*
         * We're redefinining, so we have to take over an
         * existing SMacro structure. This means freeing
         * what was already in it, but not the structure itself.
         */
        clear_smacro(smac);
    } else {
        /* Create a new macro */
        smtbl  = ctx ? &ctx->localmac : &smacros;
        smhead = (SMacro **) hash_findi_add(smtbl, mname);
        nasm_new(smac);
        smac->next = *smhead;
        *smhead = smac;
    }

    smac->name      = nasm_strdup(mname);
    smac->casesense = casesense;
    smac->expansion = expansion;
    smac->expand    = smacro_expand_default;
    if (tmpl) {
        smac->nparam     = tmpl->nparam;
        smac->params     = tmpl->params;
        smac->alias      = tmpl->alias;
        smac->greedy     = tmpl->greedy;
        if (tmpl->expand)
            smac->expand = tmpl->expand;
    }
    if (list_option('s')) {
        list_smacro_def((smac->alias ? PP_DEFALIAS : PP_DEFINE)
                        + !casesense, ctx, smac);
    }
    return smac;

fail:
    free_tlist(expansion);
    if (tmpl)
        free_smacro_members(tmpl);
    return NULL;
}