zcurrentpagedevice(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    gx_device *dev = gs_currentdevice(igs);

    push(2);
    if ((*dev_proc(dev, get_page_device))(dev) != 0) {
        op[-1] = istate->pagedevice;
        make_true(op);
    } else {
        make_null(op - 1);
        make_false(op);
    }
    return 0;
}