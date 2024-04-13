copy_stack(i_ctx_t *i_ctx_p, const ref_stack_t * pstack, int skip, ref * arr)
{
    uint size = ref_stack_count(pstack) - skip;
    uint save_space = ialloc_space(idmemory);
    int code, i;
    ref *safety, *safe;

    if (size > 65535)
        size = 65535;
    ialloc_set_space(idmemory, avm_local);
    code = ialloc_ref_array(arr, a_all, size, "copy_stack");
    if (code >= 0)
        code = ref_stack_store(pstack, arr, size, 0, 1, true, idmemory,
                               "copy_stack");
    /* If we are copying the exec stack, try to replace any oparrays with
     * with the operator than references them
     */
    if (pstack == &e_stack) {
        for (i = 0; i < size; i++) {
            if (errorexec_find(i_ctx_p, &arr->value.refs[i]) < 0)
                make_null(&arr->value.refs[i]);
        }
    }
    if (pstack == &o_stack && dict_find_string(systemdict, "SAFETY", &safety) > 0 &&
        dict_find_string(safety, "safe", &safe) > 0 && r_has_type(safe, t_boolean) &&
        safe->value.boolval == true) {
        code = ref_stack_array_sanitize(i_ctx_p, arr, arr);
        if (code < 0)
            return code;
    }
    ialloc_set_space(idmemory, save_space);
    return code;
}