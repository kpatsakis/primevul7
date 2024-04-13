static bool parse_mmacro_spec(Token *tline, MMacro *def, const char *directive)
{
    tline = tline->next;
    tline = skip_white(tline);
    tline = expand_id(tline);
    if (!tok_type(tline, TOK_ID)) {
        nasm_nonfatal("`%s' expects a macro name", directive);
        return false;
    }

#if 0
    def->prev = NULL;
#endif
    def->name = dup_text(tline);
    def->plus = false;
    def->nolist = 0;
    def->nparam_min = 0;
    def->nparam_max = 0;

    tline = expand_smacro(tline->next);
    tline = skip_white(tline);
    if (!tok_type(tline, TOK_NUMBER))
        nasm_nonfatal("`%s' expects a parameter count", directive);
    else
        def->nparam_min = def->nparam_max = read_param_count(tok_text(tline));
    if (tline && tok_is(tline->next, '-')) {
        tline = tline->next->next;
        if (tok_is(tline, '*')) {
            def->nparam_max = INT_MAX;
        } else if (!tok_type(tline, TOK_NUMBER)) {
            nasm_nonfatal("`%s' expects a parameter count after `-'", directive);
        } else {
            def->nparam_max = read_param_count(tok_text(tline));
            if (def->nparam_min > def->nparam_max) {
                nasm_nonfatal("minimum parameter count exceeds maximum");
                def->nparam_max = def->nparam_min;
            }
        }
    }
    if (tline && tok_is(tline->next, '+')) {
        tline = tline->next;
        def->plus = true;
    }
    if (tline && tok_type(tline->next, TOK_ID) &&
	tline->next->len == 7 &&
        !nasm_stricmp(tline->next->text.a, ".nolist")) {
        tline = tline->next;
        if (!list_option('f'))
            def->nolist |= NL_LIST|NL_LINE;
    }

    /*
     * Handle default parameters.
     */
    def->ndefs = 0;
    if (tline && tline->next) {
        Token **comma;
        def->dlist = tline->next;
        tline->next = NULL;
        comma = count_mmac_params(def->dlist, &def->ndefs, &def->defaults);
        if (!ppopt.sane_empty_expansion && comma) {
            *comma = NULL;
            def->ndefs--;
            nasm_warn(WARN_MACRO_PARAMS_LEGACY,
                      "dropping trailing empty default parameter in defintion of multi-line macro `%s'",
                      def->name);
        }
    } else {
        def->dlist = NULL;
        def->defaults = NULL;
    }
    def->expansion = NULL;

    if (def->defaults && def->ndefs > def->nparam_max - def->nparam_min &&
        !def->plus) {
        /*
         *!macro-defaults [on] macros with more default than optional parameters
         *!  warns when a macro has more default parameters than optional parameters.
         *!  See \k{mlmacdef} for why might want to disable this warning.
         */
        nasm_warn(WARN_MACRO_DEFAULTS,
                   "too many default macro parameters in macro `%s'", def->name);
    }

    return true;
}