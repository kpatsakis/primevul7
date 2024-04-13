zcallinstall(i_ctx_t *i_ctx_p)
{
    gx_device *dev = gs_currentdevice(igs);

    if ((dev = (*dev_proc(dev, get_page_device))(dev)) != 0) {
        int code = (*dev->page_procs.install) (dev, igs);

        if (code < 0)
            return code;
    }
    return 0;
}