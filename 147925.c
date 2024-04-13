mj_get_params(gx_device *pdev, gs_param_list *plist)
{
        int code = gdev_prn_get_params(pdev, plist);

        if ( code < 0 ||
            (code = param_write_int(plist, "Density", &mj->density)) < 0 ||
            (code = param_write_int(plist, "Cyan", &mj->cyan)) < 0 ||
            (code = param_write_int(plist, "Magenta", &mj->magenta)) < 0 ||
            (code = param_write_int(plist, "Yellow", &mj->yellow)) < 0 ||
            (code = param_write_int(plist, "Black", &mj->black)) < 0 ||
            (code = param_write_bool(plist, "Unidirectional", &mj->direction)) < 0 ||
            (code = param_write_bool(plist, "Microweave", &mj->microweave)) < 0 ||
            (code = param_write_int(plist, "DotSize", &mj->dotsize)) < 0
           )
          return code;

        return code;
}