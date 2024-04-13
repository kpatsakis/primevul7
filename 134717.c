ref_stack_set_margin(ref_stack_t *pstack, uint margin)
{
    const ref_stack_params_t *params = pstack->params;
    uint data_size = params->data_size;

    if (margin <= pstack->margin) {
        refset_null_new(pstack->top + 1, pstack->margin - margin, 0);
    } else {
        if (margin > data_size >> 1)
            return_error(gs_error_rangecheck);
        if (pstack->top - pstack->p < margin) {
            uint used = pstack->p + 1 - pstack->bot;
            uint keep = data_size - margin;
            int code = ref_stack_push_block(pstack, keep, used - keep);

            if (code < 0)
                return code;
        }
    }
    pstack->margin = margin;
    pstack->body_size = data_size - margin;
    pstack->top = pstack->bot + pstack->body_size - 1;
    return 0;
}