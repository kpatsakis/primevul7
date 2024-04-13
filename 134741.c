ref_stack_extend(ref_stack_t *pstack, uint request)
{
    uint keep = (pstack->top - pstack->bot + 1) / 3;
    uint count = pstack->p - pstack->bot + 1;
    const ref_stack_params_t *params = pstack->params;

    if (request > params->data_size)
        return_error(params->overflow_error);
    if (keep + request > pstack->body_size)
        keep = pstack->body_size - request;
    if (keep > count)
        keep = count;		/* required by ref_stack_push_block */
    return ref_stack_push_block(pstack, keep, request);
}