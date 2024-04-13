ref_stack_set_max_count(ref_stack_t *pstack, long nmax)
{
    long nmin;

    /* Bypass checking if we're setting the amximum to -1 'no limits' */
    if (nmax == -1) {
        pstack->max_stack.value.intval = nmax;
        return 0;
    }

    /* check how many items we already have on the stack, don't allow
     * a maximum less than this.
     */
    nmin = ref_stack_count_inline(pstack);

    if (nmax < nmin)
        nmax = nmin;
    if (nmax > max_uint / sizeof(ref))
        nmax = max_uint / sizeof(ref);
    if (!pstack->params->allow_expansion) {
        uint ncur = pstack->body_size;

        if (nmax > ncur)
            nmax = ncur;
    }
    pstack->max_stack.value.intval = nmax;
    return 0;
}