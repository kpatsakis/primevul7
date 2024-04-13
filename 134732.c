init_block(ref_stack_t *pstack, const ref *psb, uint used)
{
    ref_stack_params_t *params = pstack->params;
    ref *brefs = psb->value.refs;
    uint i;
    ref *p;

    for (i = params->bot_guard, p = brefs + stack_block_refs;
         i != 0; i--, p++
        )
        ref_assign(p, &params->guard_value);
    /* The top guard elements will never be read, */
    /* but we need to initialize them for the sake of the GC. */
    /* We can use refset_null for this, because even though it uses */
    /* make_null_new and stack elements must not be marked new, */
    /* these slots will never actually be read or written. */
    if (params->top_guard) {
        ref *top = brefs + r_size(psb);
        int top_guard = params->top_guard;

        refset_null_new(top - top_guard, top_guard, 0);
    } {
        ref_stack_block *const pblock = (ref_stack_block *) brefs;

        pblock->used = *psb;
        pblock->used.value.refs = brefs + stack_block_refs + params->bot_guard;
        r_set_size(&pblock->used, 0);
    }
}