znamestring(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;

    check_type(*op, t_name);
    name_string_ref(imemory, op, op);
    return 0;
}