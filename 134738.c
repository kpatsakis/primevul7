ref_stack_store_check(const ref_stack_t *pstack, ref *parray, uint count,
                      uint skip)
{
    uint space = r_space(parray);

    if (space != avm_local) {
        uint left = count, pass = skip;
        ref_stack_enum_t rsenum;

        ref_stack_enum_begin(&rsenum, pstack);
        do {
            ref *ptr = rsenum.ptr;
            uint size = rsenum.size;

            if (size <= pass)
                pass -= size;
            else {
                int code;

                if (pass != 0)
                    size -= pass, pass = 0;
                ptr += size;
                if (size > left)
                    size = left;
                left -= size;
                code = refs_check_space(ptr - size, size, space);
                if (code < 0)
                    return code;
                if (left == 0)
                    break;
            }
        } while (ref_stack_enum_next(&rsenum));
    }
    return 0;
}