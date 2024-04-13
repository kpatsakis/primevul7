ref_stack_push_block(ref_stack_t *pstack, uint keep, uint add)
{
    const ref_stack_params_t *params = pstack->params;
    uint count = pstack->p - pstack->bot + 1;
    uint move = count - keep;
    ref_stack_block *pcur = (ref_stack_block *) pstack->current.value.refs;
    ref next;
    ref_stack_block *pnext;
    ref *body;
    int code;

    if (keep > count)
        return_error(gs_error_Fatal);
    /* Check for overflowing the maximum size, */
    /* or expansion not allowed.  */
    /* Or specifically allowing unlimited expansion */
    if (pstack->max_stack.value.intval > 0) {
        if (pstack->extension_used + (pstack->top - pstack->bot) + add >=
            pstack->max_stack.value.intval ||
            !params->allow_expansion
            )
            return_error(params->overflow_error);
    }
    code = gs_alloc_ref_array(pstack->memory, &next, 0,
                              params->block_size, "ref_stack_push_block");
    if (code < 0)
        return code;
    pnext = (ref_stack_block *) next.value.refs;
    body = (ref *) (pnext + 1);
    /* Copy the top keep elements into the new block, */
    /* and make the new block the top block. */
    init_block(pstack, &next, keep);
    body += params->bot_guard;
    memcpy(body, pstack->bot + move, keep * sizeof(ref));
    /* Clear the elements above the top of the new block. */
    refset_null_new(body + keep, params->data_size - keep, 0);
    /* Clear the elements above the top of the old block. */
    refset_null_new(pstack->bot + move, keep, 0);
    pnext->next = pstack->current;
    pcur->used.value.refs = pstack->bot;
    r_set_size(&pcur->used, move);
    pstack->current = next;
    pstack->bot = body;
    pstack->top = pstack->bot + pstack->body_size - 1;
    pstack->p = pstack->bot + keep - 1;
    pstack->extension_size += pstack->body_size;
    pstack->extension_used += move;
    return 0;
}