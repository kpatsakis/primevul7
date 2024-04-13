static Token **count_mmac_params(Token *tline, int *nparamp, Token ***paramsp)
{
    int paramsize;
    int nparam = 0;
    Token *t;
    Token **comma = NULL, **maybe_comma = NULL;
    Token **params;

    paramsize = PARAM_DELTA;
    nasm_newn(params, paramsize);

    t = skip_white(tline);
    if (t) {
        while (true) {
            /* Need two slots for captured label and NULL */
            if (unlikely(nparam+2 >= paramsize)) {
                paramsize += PARAM_DELTA;
                params = nasm_realloc(params, sizeof(*params) * paramsize);
            }
            params[++nparam] = t;
            if (tok_is(t, '{')) {
                int brace = 1;

                comma = NULL;   /* Non-empty parameter */

                while (brace && (t = t->next)) {
                    brace += tok_is(t, '{');
                    brace -= tok_is(t, '}');
                }

                if (t) {
                    /*
                     * Now we've found the closing brace, look further
                     * for the comma.
                     */
                    t = skip_white(t->next);
                    if (tok_isnt(t, ','))
                        nasm_nonfatal("braces do not enclose all of macro parameter");
                } else {
                    nasm_nonfatal("expecting closing brace in macro parameter");
                }
            }

            /* Advance to the next comma */
            maybe_comma = &t->next;
            while (tok_isnt(t, ',')) {
                if (!tok_white(t))
                    comma = NULL; /* Non-empty parameter */
                maybe_comma = &t->next;
                t = t->next;
            }

            if (!t)
                break;              /* End of string, no comma */

            comma = maybe_comma;     /* Point to comma pointer */
            t = skip_white(t->next); /* Eat the comma and whitespace */
        }
    }

    params[nparam+1] = NULL;
    *paramsp = params;
    *nparamp = nparam;

    return comma;
}