static MMacro *is_mmacro(Token * tline, int *nparamp, Token ***paramsp)
{
    MMacro *head, *m, *found;
    Token **params, **comma;
    int raw_nparam, nparam;
    const char *finding = tok_text(tline);
    bool empty_args = !tline->next;

    *nparamp = 0;
    *paramsp = NULL;

    head = (MMacro *) hash_findix(&mmacros, finding);

    /*
     * Efficiency: first we see if any macro exists with the given
     * name which isn't already excluded by macro cycle removal.
     * (The cycle removal test here helps optimize the case of wrapping
     * instructions, and is cheap to do here.)
     *
     * If not, we can return NULL immediately. _Then_ we
     * count the parameters, and then we look further along the
     * list if necessary to find the proper MMacro.
     */
    list_for_each(m, head) {
        if (!mstrcmp(m->name, finding, m->casesense) &&
            (m->in_progress != 1 || m->max_depth > 0))
            break;              /* Found something that needs consideration */
    }
    if (!m)
        return NULL;

    /*
     * OK, we have a potential macro. Count and demarcate the
     * parameters.
     */
    comma = count_mmac_params(tline->next, nparamp, paramsp);
    raw_nparam = *nparamp;

    /*
     * Search for an exact match. This cannot come *before* the m
     * found in the list search before, so we can start there.
     *
     * If found is NULL and *paramsp has been cleared, then we
     * encountered an error for which we have already issued a
     * diagnostic, so we should not proceed.
     */
    found = find_mmacro_in_list(m, finding, nparamp, paramsp);
    if (!*paramsp)
        return NULL;

    nparam = *nparamp;
    params = *paramsp;

    /*
     * Special weirdness: in NASM < 2.15, an expansion of
     * *only* whitespace, as can happen during macro expansion under
     * certain circumstances, is counted as zero arguments for the
     * purpose of %0, but one argument for the purpose of macro
     * matching! In particular, this affects:
     *
     * foobar %1
     *
     * ... with %1 being empty; this would call the one-argument
     * version of "foobar" with an empty argument, equivalent to
     *
     * foobar {%1}
     *
     * ... except that %0 would be set to 0 inside foobar, even if
     * foobar is declared with "%macro foobar 1" or equivalent!
     *
     * The proper way to do that is to define "%macro foobar 0-1".
     *
     * To be compatible without doing something too stupid, try to
     * match a zero-argument macro first, but if that fails, try
     * for a one-argument macro with the above behavior.
     *
     * Furthermore, NASM < 2.15 will match stripping a tailing empty
     * argument, but in that case %0 *does* reflect that this argument
     * have been stripped; this is handled in count_mmac_params().
     *
     * To disable these insane legacy behaviors, use:
     *
     * %pragma preproc sane_empty_expansion yes
     *
     *!macro-params-legacy [on] improperly calling multi-line macro for legacy support
     *!  warns about \i{multi-line macros} being invoked
     *!  with the wrong number of parameters, but for bug-compatibility
     *!  with NASM versions older than 2.15, NASM tried to fix up the
     *!  parameters to match the legacy behavior and call the macro anyway.
     *!  This can happen in certain cases where there are empty arguments
     *!  without braces, sometimes as a result of macro expansion.
     *!-
     *!  The legacy behavior is quite strange and highly context-dependent,
     *!  and can be disabled with:
     *!-
     *!  \c      %pragma preproc sane_empty_expansion true
     *!-
     *!  It is highly recommended to use this option in new code.
     */
    if (!ppopt.sane_empty_expansion) {
        if (!found) {
            if (raw_nparam == 0 && !empty_args) {
                /*
                 * A single all-whitespace parameter as the only thing?
                 * Look for a one-argument macro, but don't adjust
                 * *nparamp.
                 */
                int bogus_nparam = 1;
                params[2] = NULL;
                found = find_mmacro_in_list(m, finding, &bogus_nparam, paramsp);
            } else if (raw_nparam > 1 && comma) {
                Token *comma_tail = *comma;

                /*
                 * Drop the terminal argument and try again.
                 * If we fail, we need to restore the comma to
                 * preserve tlist.
                 */
                *comma = NULL;
                *nparamp = raw_nparam - 1;
                found = find_mmacro_in_list(m, finding, nparamp, paramsp);
                if (found)
                    free_tlist(comma_tail);
                else
                    *comma = comma_tail;
            }

            if (!*paramsp)
                return NULL;
        } else if (comma) {
            free_tlist(*comma);
            *comma = NULL;
            if (raw_nparam > found->nparam_min &&
                raw_nparam <= found->nparam_min + found->ndefs) {
                /* Replace empty argument with default parameter */
                params[raw_nparam] =
                    found->defaults[raw_nparam - found->nparam_min];
            } else if (raw_nparam > found->nparam_max && found->plus) {
                /* Just drop the comma, don't adjust argument count */
            } else {
                /* Drop argument. This may cause nparam < nparam_min. */
                params[raw_nparam] = NULL;
                *nparamp = nparam = raw_nparam - 1;
            }
        }

        if (found) {
            if (raw_nparam < found->nparam_min ||
                (raw_nparam > found->nparam_max && !found->plus)) {
                nasm_warn(WARN_MACRO_PARAMS_LEGACY,
                          "improperly calling multi-line macro `%s' with %d parameters",
                          found->name, raw_nparam);
            } else if (comma) {
                nasm_warn(WARN_MACRO_PARAMS_LEGACY,
                          "dropping trailing empty parameter in call to multi-line macro `%s'", found->name);
            }
        }
    }

    /*
     * After all that, we didn't find one with the right number of
     * parameters. Issue a warning, and fail to expand the macro.
     *!
     *!macro-params-multi [on] multi-line macro calls with wrong parameter count
     *!  warns about \i{multi-line macros} being invoked
     *!  with the wrong number of parameters. See \k{mlmacover} for an
     *!  example of why you might want to disable this warning.
     */
    if (found)
        return found;

    nasm_warn(WARN_MACRO_PARAMS_MULTI,
               "multi-line macro `%s' exists, but not taking %d parameter%s",
              finding, nparam, (nparam == 1) ? "" : "s");
    nasm_free(*paramsp);
    return NULL;
}