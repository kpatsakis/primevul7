gs_interp_reset(i_ctx_t *i_ctx_p)
{   /* Reset the stacks. */
    ref_stack_clear(&o_stack);
    ref_stack_clear(&e_stack);
    esp++;
    make_oper(esp, 0, interp_exit);
    ref_stack_pop_to(&d_stack, min_dstack_size);
    dict_set_top();
}