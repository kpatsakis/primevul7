gs_interp_alloc_stacks(gs_ref_memory_t *mem, gs_context_state_t * pcst)
{
    int code;
    gs_ref_memory_t *smem =
        (gs_ref_memory_t *)gs_memory_stable((gs_memory_t *)mem);
    ref stk;

#define REFS_SIZE_OSTACK OS_REFS_SIZE(MAX_OSTACK)
#define REFS_SIZE_ESTACK ES_REFS_SIZE(MAX_ESTACK)
#define REFS_SIZE_DSTACK DS_REFS_SIZE(MAX_DSTACK)
    code = gs_alloc_ref_array(smem, &stk, 0,
                       REFS_SIZE_OSTACK + REFS_SIZE_ESTACK +
                       REFS_SIZE_DSTACK, "gs_interp_alloc_stacks");
    if (code < 0)
        return code;

    {
        ref_stack_t *pos = &pcst->op_stack.stack;

        r_set_size(&stk, REFS_SIZE_OSTACK);
        code = ref_stack_init(pos, &stk, OS_GUARD_UNDER, OS_GUARD_OVER, NULL,
                              smem, NULL);
	if (code < 0)
            return code;
        ref_stack_set_error_codes(pos, gs_error_stackunderflow, gs_error_stackoverflow);
        ref_stack_set_max_count(pos, MAX_OSTACK);
        stk.value.refs += REFS_SIZE_OSTACK;
    }

    {
        ref_stack_t *pes = &pcst->exec_stack.stack;
        ref euop;

        r_set_size(&stk, REFS_SIZE_ESTACK);
        make_oper(&euop, 0, estack_underflow);
        code = ref_stack_init(pes, &stk, ES_GUARD_UNDER, ES_GUARD_OVER, &euop,
                       smem, NULL);
	if (code < 0)
            return code;
        ref_stack_set_error_codes(pes, gs_error_ExecStackUnderflow,
                                  gs_error_execstackoverflow);
        /**************** E-STACK EXPANSION IS NYI. ****************/
        ref_stack_allow_expansion(pes, false);
        ref_stack_set_max_count(pes, MAX_ESTACK);
        stk.value.refs += REFS_SIZE_ESTACK;
    }

    {
        ref_stack_t *pds = &pcst->dict_stack.stack;

        r_set_size(&stk, REFS_SIZE_DSTACK);
        code = ref_stack_init(pds, &stk, 0, 0, NULL, smem, NULL);
        if (code < 0)
            return code;
        ref_stack_set_error_codes(pds, gs_error_dictstackunderflow,
                                  gs_error_dictstackoverflow);
        ref_stack_set_max_count(pds, MAX_DSTACK);
    }

#undef REFS_SIZE_OSTACK
#undef REFS_SIZE_ESTACK
#undef REFS_SIZE_DSTACK
    return 0;
}