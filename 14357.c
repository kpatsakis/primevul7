zstringbreak(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    uint i, j;

    check_read_type(op[-1], t_string);
    check_read_type(*op, t_string);
    /* We can't use strpbrk here, because C doesn't allow nulls in strings. */
    for (i = 0; i < r_size(op - 1); ++i)
        for (j = 0; j < r_size(op); ++j)
            if (op[-1].value.const_bytes[i] == op->value.const_bytes[j]) {
                make_int(op - 1, i);
                goto done;
            }
    make_null(op - 1);
 done:
    pop(1);
    return 0;
}