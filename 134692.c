ref_stack_store(const ref_stack_t *pstack, ref *parray, uint count,
                uint skip, int age, bool check, gs_dual_memory_t *idmemory,
                client_name_t cname)
{
    uint left, pass;
    ref *to;
    ref_stack_enum_t rsenum;

    if (count > ref_stack_count(pstack) || count > r_size(parray))
        return_error(gs_error_rangecheck);
    if (check) {
        int code = ref_stack_store_check(pstack, parray, count, skip);

        if (code < 0)
            return code;
    }
    to = parray->value.refs + count;
    left = count, pass = skip;
    ref_stack_enum_begin(&rsenum, pstack);
    do {
        ref *from = rsenum.ptr;
        uint size = rsenum.size;

        if (size <= pass)
            pass -= size;
        else {
            if (pass != 0)
                size -= pass, pass = 0;
            from += size;
            if (size > left)
                size = left;
            left -= size;
            switch (age) {
            case -1:		/* not an array */
                while (size--) {
                    from--, to--;
                    ref_assign(to, from);
                }
                break;
            case 0:		/* old array */
                while (size--) {
                    from--, to--;
                    ref_assign_old(parray, to, from, cname);
                }
                break;
            case 1:		/* new array */
                while (size--) {
                    from--, to--;
                    ref_assign_new(to, from);
                }
                break;
            }
            if (left == 0)
                break;
        }
    } while (ref_stack_enum_next(&rsenum));
    r_set_size(parray, count);
    return 0;
}