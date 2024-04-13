ref_stack_init(ref_stack_t *pstack, const ref *pblock_array,
               uint bot_guard, uint top_guard, const ref *pguard_value,
               gs_ref_memory_t *mem, ref_stack_params_t *params)
{
    uint size = r_size(pblock_array);
    uint avail = size - (stack_block_refs + bot_guard + top_guard);
    ref_stack_block *pblock = (ref_stack_block *)pblock_array->value.refs;
    s_ptr body = (s_ptr)(pblock + 1);

    if (params == 0) {
        params = gs_alloc_struct((gs_memory_t *)mem, ref_stack_params_t,
                                 &st_ref_stack_params,
                                 "ref_stack_alloc(stack.params)");
        if (params == 0)
            return_error(-1);	/* avoid binding in any error codes */
    }

    pstack->bot = body + bot_guard;
    pstack->p = pstack->bot - 1;
    pstack->top = pstack->p + avail;
    pstack->current = *pblock_array;
    pstack->extension_size = 0;
    pstack->extension_used = 0;

    make_int(&pstack->max_stack, avail);
    pstack->requested = 0;
    pstack->margin = 0;
    pstack->body_size = avail;

    pstack->params = params;
    pstack->memory = mem;

    params->bot_guard = bot_guard;
    params->top_guard = top_guard;
    params->block_size = size;
    params->data_size = avail;
    if (pguard_value != 0)
        params->guard_value = *pguard_value;
    else
        make_tav(&params->guard_value, t__invalid, 0, intval, 0);
    params->underflow_error = -1;
    params->overflow_error = -1;
    params->allow_expansion = true;
    init_block(pstack, pblock_array, 0);
    refset_null_new(pstack->bot, avail, 0);
    make_empty_array(&pblock->next, 0);
    return 0;
}