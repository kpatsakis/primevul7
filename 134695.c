ref_stack_index(const ref_stack_t *pstack, long idx)
{
    ref_stack_block *pblock;
    uint used = pstack->p + 1 - pstack->bot;

    if (idx < 0)
        return NULL;
    if (idx < used)		/* common case */
        return pstack->p - (uint) idx;
    pblock = (ref_stack_block *) pstack->current.value.refs;
    do {
        pblock = (ref_stack_block *) pblock->next.value.refs;
        if (pblock == 0)
            return NULL;
        idx -= used;
        used = r_size(&pblock->used);
    } while (idx >= used);
    return pblock->used.value.refs + (used - 1 - (uint) idx);
}