ref_stack_array_sanitize(i_ctx_t *i_ctx_p, ref *sarr, ref *darr)
{
    int i, code;
    ref obj, arr2;
    ref *pobj2;
    gs_memory_t *mem = (gs_memory_t *)idmemory->current;

    if (!r_is_array(sarr) || !r_has_type(darr, t_array))
        return_error(gs_error_typecheck);

    for (i = 0; i < r_size(sarr); i++) {
        code = array_get(mem, sarr, i, &obj);
        if (code < 0)
            make_null(&obj);
        switch(r_type(&obj)) {
          case t_operator:
          {
            int index = op_index(&obj);

            if (index > 0 && index < op_def_count) {
                const byte *data = (const byte *)(op_index_def(index)->oname + 1);
                if (dict_find_string(systemdict, (const char *)data, &pobj2) <= 0) {
                    byte *s = gs_alloc_bytes(mem, strlen((char *)data) + 5, "ref_stack_array_sanitize");
                    if (s) {
                        s[0] =  '\0';
                        strcpy((char *)s, "--");
                        strcpy((char *)s + 2, (char *)data);
                        strcpy((char *)s + strlen((char *)data) + 2, "--");
                    }
                    else {
                        s = (byte *)data;
                    }
                    code = name_ref(imemory, s, strlen((char *)s), &obj, 1);
                    if (code < 0) make_null(&obj);
                    if (s != data)
                        gs_free_object(mem, s, "ref_stack_array_sanitize");
                }
            }
            else {
                make_null(&obj);
            }
            ref_assign(darr->value.refs + i, &obj);
            break;
          }
          case t_array:
          case t_shortarray:
          case t_mixedarray:
          {
            int attrs = r_type_attrs(&obj) & (a_write | a_read | a_execute | a_executable);
            /* We only want to copy executable arrays */
            if (attrs & (a_execute | a_executable)) {
                code = ialloc_ref_array(&arr2, attrs, r_size(&obj), "ref_stack_array_sanitize");
                if (code < 0) {
                    make_null(&arr2);
                }
                else {
                    code = ref_stack_array_sanitize(i_ctx_p, &obj, &arr2);
                }
                ref_assign(darr->value.refs + i, &arr2);
            }
            else {
                ref_assign(darr->value.refs + i, &obj);
            }
            break;
          }
          default:
            ref_assign(darr->value.refs + i, &obj);
        }
    }
    return 0;
}