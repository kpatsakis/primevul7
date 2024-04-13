static MMacro *use_mmacro(MMacro *m, int *nparamp, Token ***paramsp)
{
    int nparam = *nparamp;
    Token **params = *paramsp;

    /*
     * This one is right. Just check if cycle removal
     * prohibits us using it before we actually celebrate...
     */
    if (m->in_progress > m->max_depth) {
        if (m->max_depth > 0) {
            nasm_warn(WARN_OTHER, "reached maximum recursion depth of %i",
                      m->max_depth);
        }
        nasm_free(params);
        *nparamp = 0;
        *paramsp = NULL;
        return NULL;
    }

    /*
     * It's right, and we can use it. Add its default
     * parameters to the end of our list if necessary.
     */
    if (m->defaults && nparam < m->nparam_min + m->ndefs) {
        int newnparam = m->nparam_min + m->ndefs;
        params = nasm_realloc(params, sizeof(*params) * (newnparam+2));
        memcpy(&params[nparam+1], &m->defaults[nparam+1-m->nparam_min],
               (newnparam - nparam) * sizeof(*params));
        nparam = newnparam;
    }
    /*
     * If we've gone over the maximum parameter count (and
     * we're in Plus mode), ignore parameters beyond
     * nparam_max.
     */
    if (m->plus && nparam > m->nparam_max)
        nparam = m->nparam_max;

    /*
     * If nparam was adjusted above, make sure the list is still
     * NULL-terminated.
     */
    params[nparam+1] = NULL;

    /* Done! */
    *paramsp = params;
    *nparamp = nparam;
    return m;
}