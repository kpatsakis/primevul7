zcallendpage(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    gx_device *dev = gs_currentdevice(igs);
    int code;

    check_type(op[-1], t_integer);
    check_type(*op, t_integer);
    if ((dev = (*dev_proc(dev, get_page_device))(dev)) != 0) {
        code = (*dev->page_procs.end_page)(dev, (int)op->value.intval, igs);
        if (code < 0)
            return code;
        if (code > 1)
            return_error(gs_error_rangecheck);
    } else {
        code = (op->value.intval == 2 ? 0 : 1);
    }
    make_bool(op - 1, code);
    pop(1);
    return 0;
}