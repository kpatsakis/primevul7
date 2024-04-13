zanchorsearch(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    os_ptr op1 = op - 1;
    uint size = r_size(op);

    check_read_type(*op, t_string);
    check_read_type(*op1, t_string);
    if (size <= r_size(op1) && !memcmp(op1->value.bytes, op->value.bytes, size)) {
        os_ptr op0 = op;

        push(1);
        *op0 = *op1;
        r_set_size(op0, size);
        op1->value.bytes += size;
        r_dec_size(op1, size);
        make_true(op);
    } else
        make_false(op);
    return 0;
}