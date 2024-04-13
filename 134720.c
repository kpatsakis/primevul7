oparray_cleanup(i_ctx_t *i_ctx_p)
{                               /* esp points just below the cleanup procedure. */
    es_ptr ep = esp;
    uint ocount_old = (uint) ep[3].value.intval;
    uint dcount_old = (uint) ep[4].value.intval;
    uint ocount = ref_stack_count(&o_stack);
    uint dcount = ref_stack_count(&d_stack);

    if (ocount > ocount_old)
        ref_stack_pop(&o_stack, ocount - ocount_old);
    if (dcount > dcount_old) {
        ref_stack_pop(&d_stack, dcount - dcount_old);
        dict_set_top();
    }
    return 0;
}