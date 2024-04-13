ref_stack_counttomark(const ref_stack_t *pstack)
{
    uint scanned = 0;
    ref_stack_enum_t rsenum;

    ref_stack_enum_begin(&rsenum, pstack);
    do {
        uint count = rsenum.size;
        const ref *p = rsenum.ptr + count - 1;

        for (; count; count--, p--)
            if (r_has_type(p, t_mark))
                return scanned + (rsenum.size - count + 1);
        scanned += rsenum.size;
    } while (ref_stack_enum_next(&rsenum));
    return 0;
}