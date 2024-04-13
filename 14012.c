lprn_put_params(gx_device * dev, gs_param_list * plist)
{
    gx_device_lprn *const lprn = (gx_device_lprn *) dev;
    int ecode = 0;
    int code;
    gs_param_name param_name;
    bool ManualFeed = lprn->ManualFeed;
    bool NegativePrint = lprn->NegativePrint;
    bool Tumble = lprn->Tumble;
    bool RITOff = lprn->RITOff;
    int BlockLine = lprn->BlockLine;
    int BlockWidth = lprn->nBw;
    int BlockHeight = lprn->nBh;
    bool ShowBubble = lprn->ShowBubble;

    if ((code = param_read_bool(plist,
                                (param_name = "ManualFeed"),
                                &ManualFeed)) < 0) {
        param_signal_error(plist, param_name, ecode = code);
    }

    if ((code = param_read_bool(plist,
                                (param_name = "NegativePrint"),
                                &NegativePrint)) < 0) {
        param_signal_error(plist, param_name, ecode = code);
    }
    if ((code = param_read_bool(plist,
                                (param_name = "Tumble"),
                                &Tumble)) < 0) {
        param_signal_error(plist, param_name, ecode = code);
    }
    if ((code = param_read_bool(plist,
                                (param_name = "RITOff"),
                                &RITOff)) < 0) {
        param_signal_error(plist, param_name, ecode = code);
    }
    switch (code = param_read_int(plist,
                                  (param_name = "BlockWidth"),
                                  &BlockWidth)) {
        case 0:
            if (BlockWidth < 0)
                ecode = gs_error_rangecheck;
            else
                break;
            goto bwidthe;
        default:
            ecode = code;
          bwidthe:param_signal_error(plist, param_name, ecode = code);
        case 1:
            break;
    }

    switch (code = param_read_int(plist,
                                  (param_name = "BlockLine"),
                                  &BlockLine)) {
        case 0:
            if (BlockLine < 0)
                ecode = gs_error_rangecheck;
            else
                break;
            goto crowe;
        default:
            ecode = code;
          crowe:param_signal_error(plist, param_name, ecode = code);
        case 1:
            break;
    }

    switch (code = param_read_int(plist,
                                  (param_name = "BlockHeight"),
                                  &BlockHeight)) {
        case 0:
            if (BlockHeight < 0)
                ecode = gs_error_rangecheck;
            else
                break;
            goto bheighte;
        default:
            ecode = code;
          bheighte:param_signal_error(plist, param_name, ecode = code);
        case 1:
            break;
    }

    if ((code = param_read_bool(plist,
                                (param_name = "ShowBubble"),
                                &ShowBubble)) < 0) {
        param_signal_error(plist, param_name, ecode = code);
    }
    if (ecode < 0)
        return ecode;
    code = gdev_prn_put_params(dev, plist);
    if (code < 0)
        return code;

    lprn->ManualFeed = ManualFeed;
    lprn->NegativePrint = NegativePrint;
    lprn->Tumble = Tumble;
    lprn->RITOff = RITOff;
    lprn->BlockLine = BlockLine;
    lprn->nBw = BlockWidth;
    lprn->nBh = BlockHeight;
    lprn->ShowBubble = ShowBubble;

    return 0;
}