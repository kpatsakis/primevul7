zstringmatch(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    os_ptr op1 = op - 1;
    bool result;

    check_read_type(*op, t_string);
    switch (r_type(op1)) {
        case t_string:
            check_read(*op1);
            goto cmp;
        case t_name:
            name_string_ref(imemory, op1, op1);	/* can't fail */
cmp:
            result = string_match(op1->value.const_bytes, r_size(op1),
                                  op->value.const_bytes, r_size(op),
                                  NULL);
            break;
        default:
            result = (r_size(op) == 1 && *op->value.bytes == '*');
    }
    make_bool(op1, result);
    pop(1);
    return 0;
}