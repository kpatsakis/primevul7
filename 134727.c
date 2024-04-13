ref_stack_pop_block(ref_stack_t *pstack)
{
    s_ptr bot = pstack->bot;
    uint count = pstack->p + 1 - bot;
    ref_stack_block *pcur =
    (ref_stack_block *) pstack->current.value.refs;
    ref_stack_block *pnext =
    (ref_stack_block *) pcur->next.value.refs;
    uint used;
    ref *body;
    ref next;

    if (pnext == 0)
        return_error(pstack->params->underflow_error);
    used = r_size(&pnext->used);
    body = (ref *) (pnext + 1) + pstack->params->bot_guard;
    next = pcur->next;
    /*
       * If the contents of the two blocks won't fit in a single block, we
       * move up the used part of the top block, and copy up as much of
       * the contents of the next block under it as will fit.  If the
       * contents of both blocks fit in a single block, we copy the used
       * part of the top block to the top of the next block, and free the
       * top block.
     */
    if (used + count > pstack->body_size) {
        /*
         * The contents of the two blocks won't fit into a single block.
         * On the assumption that we're recovering from a local stack
         * underflow and need to increase the number of contiguous
         * elements available, move up the used part of the top block, and
         * copy up as much of the contents of the next block under it as
         * will fit.
         */
        uint moved = pstack->body_size - count;
        uint left;

        if (moved == 0)
            return_error(gs_error_Fatal);
        memmove(bot + moved, bot, count * sizeof(ref));
        left = used - moved;
        memcpy(bot, body + left, moved * sizeof(ref));
        refset_null_new(body + left, moved, 0);
        r_dec_size(&pnext->used, moved);
        pstack->p = pstack->top;
        pstack->extension_used -= moved;
    } else {
        /*
           * The contents of the two blocks will fit into a single block.
           * Copy the used part of the top block to the top of the next
           * block, and free the top block.
         */
        memcpy(body + used, bot, count * sizeof(ref));
        pstack->bot = bot = body;
        pstack->top = bot + pstack->body_size - 1;
        gs_free_ref_array(pstack->memory, &pstack->current,
                          "ref_stack_pop_block");
        pstack->current = next;
        pstack->p = bot + (used + count - 1);
        pstack->extension_size -= pstack->body_size;
        pstack->extension_used -= used;
    }
    return 0;
}