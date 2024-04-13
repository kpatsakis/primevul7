zcurrentshowpagecount(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    gx_device *dev1, *dev = gs_currentdevice(igs);

    if ((*dev_proc(dev, get_page_device))(dev) == 0) {
        push(1);
        make_false(op);
    } else {
        dev1 = (*dev_proc(dev, get_page_device))(dev);
        push(2);
        make_int(op - 1, dev1->ShowpageCount);
        make_true(op);
    }
    return 0;
}