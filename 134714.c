ref_stack_push(ref_stack_t *pstack, uint count)
{
    /* Don't bother to pre-check for overflow: we must be able to */
    /* back out in the case of a VMerror anyway, and */
    /* ref_stack_push_block will make the check itself. */
    uint needed = count;
    uint added;

    for (; (added = pstack->top - pstack->p) < needed; needed -= added) {
        int code;

        pstack->p = pstack->top;
        code = ref_stack_push_block(pstack,
                                    (pstack->top - pstack->bot + 1) / 3,
                                    added);
        if (code < 0) {
            /* Back out. */
            ref_stack_pop(pstack, count - needed + added);
            pstack->requested = count;
            return code;
        }
    }
    pstack->p += needed;
    return 0;
}