lprn_get_params(gx_device * dev, gs_param_list * plist)
{
    gx_device_lprn *const lprn = (gx_device_lprn *) dev;
    int code = gdev_prn_get_params(dev, plist);
    int ncode;

    if (code < 0)
        return code;

    if ((ncode = param_write_bool(plist, "ManualFeed", &lprn->ManualFeed)) < 0)
        code = ncode;

    if ((ncode = param_write_bool(plist, "NegativePrint", &lprn->NegativePrint)) < 0)
        code = ncode;

    if ((ncode = param_write_bool(plist, "Tumble", &lprn->Tumble)) < 0)
        code = ncode;

    if ((ncode = param_write_bool(plist, "RITOff", &lprn->RITOff)) < 0)
        code = ncode;

    if ((ncode = param_write_int(plist, "BlockLine", &lprn->BlockLine)) < 0)
        code = ncode;

    if ((ncode = param_write_int(plist, "BlockWidth", &lprn->nBw)) < 0)
        code = ncode;

    if ((ncode = param_write_int(plist, "BlockHeight", &lprn->nBh)) < 0)
        code = ncode;

    if ((ncode = param_write_bool(plist, "ShowBubble", &lprn->ShowBubble)) < 0)
        code = ncode;

    return code;
}