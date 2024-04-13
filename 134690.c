ref_stack_cleanup(ref_stack_t *pstack)
{
    ref_stack_block *pblock =
        (ref_stack_block *) pstack->current.value.refs;

    refset_null_new(pstack->p + 1, pstack->top - pstack->p, 0);
    pblock->used = pstack->current;	/* set attrs */
    pblock->used.value.refs = pstack->bot;
    r_set_size(&pblock->used, pstack->p + 1 - pstack->bot);
}